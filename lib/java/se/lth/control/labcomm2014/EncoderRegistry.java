package se.lth.control.labcomm2014;

import java.io.IOException;
import java.util.HashMap;

public class EncoderRegistry {

  public static class Entry {
    
    private SampleDispatcher dispatcher;
    private int index;

    public Entry(SampleDispatcher dispatcher, int index) {
      this.dispatcher = dispatcher;
      this.index = index;
    }

    public SampleDispatcher getDispatcher() {
      return dispatcher;
    }

    public int getIndex() {
      return index;
    }

  }

  private int userIndex = Constant.FIRST_USER_INDEX;
  private HashMap<SampleDispatcher, Entry> byDispatcher;

  public EncoderRegistry() {
    byDispatcher = new HashMap<SampleDispatcher, Entry>();
  }

  public synchronized int add(SampleDispatcher dispatcher) {
    Entry e = byDispatcher.get(dispatcher);
    if (e == null) {
      e = new Entry(dispatcher, userIndex);
      byDispatcher.put(dispatcher, e);
      userIndex++;
    }
    return e.getIndex();
  }
  
  public synchronized int getTag(SampleDispatcher sample) throws IOException {
    Entry e = byDispatcher.get(sample);
    if (e == null) {
      throw new IOException("'" + 
			    sample.getName() + 
			    "' is not registered");
    }
    return e.getIndex();
  }

}
