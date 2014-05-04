package com.plotlet.element.plotgrid;

import java.util.Arrays;
import java.util.List;

import com.baselet.diagram.draw.BaseDrawHandler;
import com.baselet.diagram.draw.swing.objects.PlotGridDrawConfig;
import com.plotlet.parser.PlotConstants;
import com.plotlet.parser.PlotState;

public class ScatterPlot extends AbstractPlot {

	public ScatterPlot(BaseDrawHandler drawer, PlotGridDrawConfig plotDrawConfig, PlotState plotState, int xPos, int yPos) {
		super(drawer, plotDrawConfig, plotState, xPos, yPos); 
	}

	@Override
	public void plot(int columnCount, int rowCount) {
		setPlotPosition(columnCount, rowCount);
		Boolean tilt = plotState.getValueAsBoolean(PlotConstants.KEY_BOOL_PLOT_TILT, PlotConstants.PLOT_TILT_DEFAULT);
		plotState.checkIfAllValuesUsed();
		plot.drawPlotAndDescValueAxis(!tilt, false, false, true);
	}

	@Override
	protected List<String> defaultDescAxisShow() {
		return Arrays.asList(new String[] {PlotConstants.DESC_AXIS_SHOW_AXIS, PlotConstants.DESC_AXIS_SHOW_MARKER, PlotConstants.DESC_AXIS_SHOW_TEXT});
	}

	@Override
	protected List<String> defaultValueAxisShow() {
		return Arrays.asList(new String[] {PlotConstants.VALUE_AXIS_SHOW_AXIS, PlotConstants.VALUE_AXIS_SHOW_MARKER, PlotConstants.VALUE_AXIS_SHOW_TEXT});
	}

	@Override
	protected List<String> defaultValueAxisList() {
		return Arrays.asList(new String[] {PlotConstants.VALUE_AXIS_LIST_RELEVANT});
	}

	@Override
	protected int getMaxAllowedValueRows() {
		return Integer.MAX_VALUE;
	}

}
