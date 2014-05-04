package com.umlet.custom;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.log4j.Logger;

import com.baselet.control.Constants;
import com.baselet.control.Constants.SystemInfo;
import com.baselet.control.Path;
import com.baselet.element.ErrorOccurred;
import com.baselet.element.GridElement;

public class CustomElementCompiler {
	
	private static final Logger log = Logger.getLogger(CustomElementCompiler.class);

	private static CustomElementCompiler compiler;
	private static final String templatefile = "Default.java";
	private String template;
	private Pattern template_pattern;
	private Matcher template_match;
	private String classname;
	private int beforecodelines; // lines of code before the custom code begins (for error processing)
	private List<CompileError> compilation_errors;
	private boolean global_error;

	public static CustomElementCompiler getInstance() {
		if (compiler == null) compiler = new CustomElementCompiler();
		return compiler;
	}

	private File sourcefile;

	private CustomElementCompiler() {
		this.global_error = false;
		this.compilation_errors = new ArrayList<CompileError>();
		this.beforecodelines = 0;
		this.template_pattern = Pattern.compile("(.*)(/\\*\\*\\*\\*CUSTOM_CODE START\\*\\*\\*\\*/\n)(.*)(\n\\s\\s/\\*\\*\\*\\*CUSTOM_CODE END\\*\\*\\*\\*/)(.*)", Pattern.DOTALL);
		this.template = this.loadJavaSource(new File(Path.customElements() + templatefile));
		if (!"".equals(this.template)) {
			this.template_match = this.template_pattern.matcher(this.template);
			try {
				if (template_match.matches()) beforecodelines = this.template_match.group(1).split(Constants.NEWLINE).length;
				else this.global_error = true;
			} catch (Exception ex) {
				ex.printStackTrace();
			}
		}
		else global_error = true;

		this.classname = Constants.CUSTOM_ELEMENT_CLASSNAME;
		this.sourcefile = new File(Path.temp() + this.classname + ".java");
		sourcefile.deleteOnExit();
		new File(Path.temp() + this.classname + ".class").deleteOnExit();
	}

	// compiles the element and returns the new entity if successful
	private CustomElement compile(String code) {
		this.saveJavaSource(code);
		CustomElement entity = null;
		this.compilation_errors.clear();
		try {
			StringWriter compilerErrorMessageSW = new StringWriter(); // catch compiler messages
			PrintWriter compilerErrorMessagePW = new PrintWriter(compilerErrorMessageSW);
			String path = Path.executable();

			String javaVersion = "-\"" + SystemInfo.JAVA_VERSION + "\"";
			String classpath = "-classpath \"" + path + "\"" + File.pathSeparator + "\"" + path + "bin/\"";
			String sourcefile = "\"" + this.sourcefile.getAbsolutePath() + "\"";

			// Compiler Information at http://dev.eclipse.org/viewcvs/index.cgi/jdt-core-home/howto/batch%20compile/batchCompile.html?revision=1.7
			@SuppressWarnings("deprecation")
			boolean compilationSuccessful = org.eclipse.jdt.internal.compiler.batch.Main.compile(
					javaVersion + " " + classpath + " " + sourcefile,
					new PrintWriter(System.out),
					compilerErrorMessagePW);

			if (compilationSuccessful) {
				FileClassLoader fcl = new FileClassLoader(Thread.currentThread().getContextClassLoader());
				Class<?> c = fcl.findClass(this.classname); // load class by type name
				if (c != null) entity = (CustomElement) c.newInstance();
			}
			else {
				this.compilation_errors = CompileError.getListFromString(compilerErrorMessageSW.toString(), beforecodelines);
			}
		} catch (Exception e) {
			log.error(null, e);
		}
		if (entity == null) entity = new CustomElementWithErrors(compilation_errors);
		return entity;
	}

	// loads the source from a file
	private String loadJavaSource(File sourceFile) { // LME3
		String _javaSource = "";
		if ((sourceFile != null) && (sourceFile.getName().endsWith(".java"))) {
			try {
				BufferedReader br = new BufferedReader(new FileReader(sourceFile));
				String line;
				while ((line = br.readLine()) != null) {
					_javaSource += line + Constants.NEWLINE;
				}
				br.close();
			} catch (Exception e) {
				log.error(null, e);
			}
		}
		return _javaSource.replaceAll("\r\n", Constants.NEWLINE);
	}

	// saves the source to a file
	private void saveJavaSource(String code) { // LME3
		BufferedWriter bw = null;
		try {
			bw = new BufferedWriter(new FileWriter(sourcefile, false));
			bw.write(this.parseCodeIntoTemplate(code));
			bw.flush();
		} catch (IOException e) {
			log.error(null, e);
		} finally {
			if (bw != null) try {
				bw.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}

	private String parseCodeFromTemplate(String template) {
		Matcher m = this.template_pattern.matcher(template);
		if (m.matches()) return m.group(3);
		else return "";
	}

	private String parseCodeIntoTemplate(String code) {
		return template_match.group(1).replaceFirst("<!CLASSNAME!>", this.classname) +
				template_match.group(2) +
				code +
				template_match.group(4) +
				template_match.group(5);
	}

	public GridElement genEntity(String code, ErrorHandler errorhandler) {
		if (!Constants.enable_custom_elements) return new ErrorOccurred("Custom Elements are disabled\nEnable them in the Options\nOnly open Custom Elements\nfrom trusted sources!");
		if (this.global_error) return new ErrorOccurred();

		if (code == null) code = this.parseCodeFromTemplate(this.template);

		CustomElement element = this.compile(code);
		if (errorhandler != null) errorhandler.addErrors(compilation_errors);
		element.setCode(code);
		return element;
	}

	public GridElement genEntity(String code) {
		return this.genEntity(code, null);
	}

	public GridElement genEntityFromTemplate(String templatename, ErrorHandler errorhandler) {
		String template = this.loadJavaSource(new File(Path.customElements() + templatename + ".java"));
		if (!"".equals(template)) return this.genEntity(this.parseCodeFromTemplate(template), errorhandler);

		return null;
	}
}
