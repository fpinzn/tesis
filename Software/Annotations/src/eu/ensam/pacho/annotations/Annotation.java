package eu.ensam.pacho.annotations;

public class Annotation {
	
	//This fields need to be public so they can be accessed from the VrpnClient trough introspection.
	public String author;
	public String content;
	public Integer priority;
	public Long id;
	public String date;

	
	public Annotation() {
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
	public long getId() {
		return id;
	}
	public void setId(long id) {
		this.id = id;
	}
	public void setDate(String date){
		this.date=date;
	}

	public String getDate() {
		return date;
	}

}
