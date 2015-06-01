package se.lth.control.labcomm2014;

import java.io.IOException;
import java.util.HashMap;

public class DecoderRegistry {

  public static class Entry {
    
    private SampleDispatcher dispatcher;
    private SampleHandler handler;
    private int index;
    private String name;
    private byte[] signature;

    public Entry(SampleDispatcher dispatcher,
		 SampleHandler handler) {
      this.dispatcher = dispatcher;
      this.name = dispatcher.getName();
      this.signature = dispatcher.getSignature();
      this.handler = handler;
    }

    public Entry(int index, String name, byte[] signature) {
      this.index = index;
      this.name = name;
      this.signature = signature;
    }

    public SampleDispatcher getDispatcher() {
      return dispatcher;
    }

    public void setDispatcher(SampleDispatcher dispatcher) {
      this.dispatcher = dispatcher;
    }

    public SampleHandler getHandler() {
      return handler;
    }

    public void setHandler(SampleHandler handler) {
      this.handler = handler;
    }

    public String getName() {
      return name;
    }

    public int getIndex() {
      return index;
    }

    // protected, for TypeDefParser...
    byte[] getSignature() {
        return signature;
    }

    public void setIndex(int index) throws IOException {
      if (this.index != 0 && this.index != index) {
	throw new IOException("Index mismatch " + 
			      this.index + " != " + index);
      }
      this.index = index;
    }

    public boolean matchName(String name) {
      return this.name.equals(name);
    }

    public boolean matchSignature(byte[] signature) {
      boolean result = this.signature.length == signature.length;
      for (int i = 0 ; result && i < signature.length ; i++) {
	result = this.signature[i] == signature[i];
      }
      return result;
    }

    public boolean match(String name, byte[] signature) {
      return matchName(name) && matchSignature(signature);
    }

    public void check(String name, byte[] signature) throws IOException {
      if (!matchName(name)) {
	throw new IOException("Name mismatch '" + 
			      this.name + "' != '" + name + "'");
      } 
      if (!matchSignature(signature)) {
	throw new IOException("Signature mismatch");
      } 
    }
  }

  private HashMap<SampleDispatcher, Entry> byDispatcher;
  private HashMap<Integer, Entry> byIndex;

  public DecoderRegistry() {
    byDispatcher = new HashMap<SampleDispatcher, Entry>();
    byIndex = new HashMap<Integer, Entry>();
  }

  public synchronized void add(SampleDispatcher dispatcher,
			       SampleHandler handler) throws IOException{
    Entry e = byDispatcher.get(dispatcher);
    if (e != null) {
      e.check(dispatcher.getName(), dispatcher.getSignature());
      e.setHandler(handler);
    } else {
      for (Entry e2 : byIndex.values()) {
	if (e2.match(dispatcher.getName(), dispatcher.getSignature())) {
	  e2.setDispatcher(dispatcher);
	  e2.setHandler(handler);
	  e = e2;
	  break;
	}
      }
      if (e == null) {
	e = new Entry(dispatcher, handler);
	byDispatcher.put(dispatcher, e);
      }
    }
  }

  public synchronized void add(int index, 
			       String name,
			       byte[] signature) throws IOException {
    Entry e = byIndex.get(Integer.valueOf(index));
    if (e != null) {
      e.check(name, signature);
    } else {
      for (Entry e2 : byDispatcher.values()) {
	if (e2.match(name, signature)) {
	  e2.setIndex(index);
	  e = e2;
	  break;
	}
      }
      if (e == null) {
	e = new Entry(index, name, signature);
      }
      byIndex.put(Integer.valueOf(index), e);
    }
  }

  public synchronized Entry get(int index) {
    return byIndex.get(Integer.valueOf(index));
  }

}
