package com.umlet.custom;

import java.util.Iterator;
import java.util.Timer;
import java.util.TimerTask;

import com.baselet.control.Constants;
import com.baselet.control.Constants.Program;
import com.baselet.control.Main;
import com.baselet.diagram.CustomPreviewHandler;
import com.baselet.diagram.DiagramHandler;
import com.baselet.diagram.DrawPanel;
import com.baselet.diagram.draw.geom.Rectangle;
import com.baselet.element.GridElement;
import com.umlet.element.experimental.ElementFactory;
import com.umlet.gui.CustomCodeSyntaxPane;
import com.umlet.gui.CustomElementPanel;


public class CustomElementHandler {

	private Timer timer;
	private CustomCodeSyntaxPane codepane;
	private CustomPreviewHandler preview;
	private GridElement editedEntity;
	private GridElement originalElement;
	private TimerTask compiletask;
	private boolean changed;
	private ErrorHandler errorhandler;
	private boolean compilation_running;
	private CustomElementPanel panel;
	boolean keypressed;
	private String old_text;

	public CustomElementHandler() {
		this.codepane = new CustomCodeSyntaxPane();
		this.errorhandler = new ErrorHandler(this.codepane);
		this.codepane.getTextComponent().addMouseMotionListener(this.errorhandler);
		this.preview = new CustomPreviewHandler();
		this.timer = new Timer(true);
		this.changed = false;
		this.compilation_running = false;
		this.old_text = null;
		this.panel = new CustomElementPanel(this);
	}

	public CustomElementPanel getPanel() {
		return this.panel;
	}

	public void newEntity() {
		this.newEntity("Default");
	}

	public void newEntity(String template) {
		this.preview.closePreview();
		this.originalElement = null;
		this.editedEntity = CustomElementCompiler.getInstance().genEntityFromTemplate(template, this.errorhandler);
		if (editedEntity instanceof CustomElement) this.codepane.setCode(((CustomElement) this.editedEntity).getCode());
		else {
			this.codepane.setCode("");
		}
		this.editedEntity.setPanelAttributes("// Modify the text below and" +
				Constants.NEWLINE +
				"// observe the element preview." +
				Constants.NEWLINE + Constants.NEWLINE +
				"Hello, World! " +
				Constants.NEWLINE +
				"Enjoy " + Program.PROGRAM_NAME + "!");
		this.editedEntity.setRectangle(new Rectangle(20, 20, 200, 200));
		this.updatePreview(editedEntity);
		this.getPreviewHandler().getDrawPanel().getSelector().select(editedEntity);
		this.setChanged(false);
		this.start();
	}

	public void editEntity(CustomElement e) {
		this.preview.closePreview();
		this.originalElement = e;
		this.editedEntity = e.CloneFromMe();
		this.editedEntity.setLocation(20, 20);
		this.codepane.setCode(e.getCode());
		this.updatePreview(this.editedEntity);
		this.getPreviewHandler().getDrawPanel().getSelector().select(editedEntity);
		this.setChanged(false);

		this.start();
	}

	public void saveEntity() {
		GridElement e = CustomElementCompiler.getInstance().genEntity(this.codepane.getText(), errorhandler);
		this.editedEntity = e;
		this.updatePreview(e); // update preview panel to set the entities bounds...
		this.updateElement(e);
		this.setChanged(false);
	}

	public boolean closeEntity() {
		if (this.changed) {
			/*
			 * int ch=JOptionPane.showOptionDialog(Main.getInstance().getGUI(),"Save changes?","CustomElement",JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.WARNING_MESSAGE,null, null, null);
			 * if (ch==JOptionPane.YES_OPTION)
			 * this.saveEntity();
			 * else if (ch==JOptionPane.CANCEL_OPTION)
			 * return false;
			 * else
			 * this.setChanged(false);
			 */
			this.setChanged(false);
		}
		this.stop();
		this.preview.closePreview();
		this.preview.getDrawPanel().getSelector().deselectAll();

		//clear controller before editing new custom element
		Main.getInstance().getDiagramHandler().getController().clear();
		
		DrawPanel dia = Main.getInstance().getGUI().getCurrentDiagram();
		if (dia != null) dia.getSelector().updateSelectorInformation();
		else Main.getInstance().setPropertyPanelToGridElement(null);
		return true;
	}

	public CustomPreviewHandler getPreviewHandler() {
		return this.preview;
	}

	public CustomCodeSyntaxPane getCodePane() {
		return this.codepane;
	}

	private void updatePreview(GridElement e) {
		if (e != null) {
			Iterator<GridElement> iter = this.preview.getDrawPanel().getAllEntities().iterator();
			if (iter.hasNext()) {
				GridElement element = iter.next();
				e.setRectangle(element.getRectangle());
				e.setPanelAttributes(element.getPanelAttributes());
				if (this.preview.getDrawPanel().getSelector().getSelectedElements().size() > 0) this.preview.getDrawPanel().getSelector().singleSelectWithoutUpdatePropertyPanel(e);
				this.preview.getDrawPanel().removeElement(element);
			}

			this.preview.setHandlerAndInitListeners(e);
			this.preview.getDrawPanel().addElement(e);
			e.repaint();
		}
	}

	// starts the task
	private void start() {
		this.compiletask = new CustomElementCompileTask(this);
		this.timer.schedule(compiletask, Constants.CUSTOM_ELEMENT_COMPILE_INTERVAL,
				Constants.CUSTOM_ELEMENT_COMPILE_INTERVAL);
	}

	// stops the task
	private void stop() {
		if (this.compiletask != null) this.compiletask.cancel();
	}

	// runs compilation every 1 seconds and updates gui/errors...
	protected void runCompilation() {
		if (!this.compilation_running && !keypressed) // prevent 2 compilations to run at the same time (if compilation takes more then 1sec)
		{
			this.compilation_running = true;
			String txt = this.codepane.getText();
			if (!txt.equals(this.old_text)) {
				this.setChanged(true);
				this.errorhandler.clearErrors();
				this.old_text = txt;
				GridElement e = CustomElementCompiler.getInstance().genEntity(txt, errorhandler);
				if (e != null) {
					this.editedEntity = e;
					this.panel.setCustomElementSaveable(true);
					this.updatePreview(e);
				}
				else {
					this.panel.setCustomElementSaveable(false);
				}
			}
			this.compilation_running = false;
		}
		keypressed = false;
	}

	private void setChanged(boolean changed) {
		this.changed = changed;
		Main.getInstance().getGUI().setCustomElementChanged(this, changed);
	}

	// reloads the element on all open panels and adds it to the custom element panel if not already there.
	private void updateElement(GridElement element) {

		// if a new element has been created add it to current diagram
		if (this.originalElement == null) {
			DiagramHandler current = null;
			DrawPanel c = Main.getInstance().getGUI().getCurrentDiagram();
			if (c == null) {
				Main.getInstance().doNew();
				current = Main.getInstance().getGUI().getCurrentDiagram().getHandler();
			}
			else current = c.getHandler();

			// set location for element
			int x = 10, y = 10;
			for (GridElement e : current.getDrawPanel().getAllEntities()) {
				if (e.getRectangle().y + e.getRectangle().height + 10 > y) y = e.getRectangle().y + e.getRectangle().height + 10;
			}

			Rectangle bounds = new Rectangle(x, y, element.getRectangle().width, element.getRectangle().height);
			this.addElementToDiagram(element, current, true, bounds, element.getPanelAttributes());
		}
		else { // replace edited element (and ONLY edited element)
			Main.getHandlerForElement(this.originalElement).getDrawPanel().removeElement(this.originalElement);
			this.addElementToDiagram(element, Main.getHandlerForElement(this.originalElement), true,
					this.originalElement.getRectangle(), this.originalElement.getPanelAttributes());
		}
	}

	private void addElementToDiagram(GridElement e, DiagramHandler d, boolean setchanged, Rectangle bounds, String state) {

		// TODO bug if custom elements get inserted in zoomed drawpanel
		// the zoom to 100% bugfix works for inserting new elements but editing old elements with zoom will not work anymore
		// We must zoom to the defaultGridsize before execution
		// int oldZoom = d.getGridSize();
		// d.setGridAndZoom(Constants.DEFAULTGRIDSIZE, false);

		GridElement e2 = ElementFactory.createCopy(e);
		d.setHandlerAndInitListeners(e2);
		e2.setPanelAttributes(state);
		e2.setRectangle(bounds);
		d.getDrawPanel().addElement(e2);
		if (setchanged) d.setChanged(true);

		// And zoom back to the oldGridsize after execution
		// d.setGridAndZoom(oldZoom, false);
	}
}
