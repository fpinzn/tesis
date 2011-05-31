package eu.ensam.pacho.annotations;

import java.util.Date;

public class Annotation {
	
	private String author;
	private String content;
	private int priority;
	private int id;
	private Date date;
	public Annotation(String author, String content, int priority, int id) {
		super();
		this.author = author;
		this.content = content;
		this.priority = priority;
		this.id = id;
	}
	
	public String getAuthor() {
		return author;
	}
	public void setAuthor(String author) {
		this.author = author;
	}
	public String getContent() {
		return content;
	}
	public void setContent(String content) {
		this.content = content;
	}
	public int getPriority() {
		return priority;
	}
	public void setPriority(int priority) {
		this.priority = priority;
	}
	public int getId() {
		return id;
	}
	public void setId(int id) {
		this.id = id;
	}


}
