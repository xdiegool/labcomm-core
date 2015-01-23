package se.lth.control.labcomm;

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
  private HashMap<Class, Entry> byClass;

  public EncoderRegistry() {
    byClass = new HashMap<Class, Entry>();
  }

  public synchronized int add(SampleDispatcher dispatcher) {
    Entry e = byClass.get(dispatcher.getSampleClass());
    if (e == null) {
      e = new Entry(dispatcher, userIndex);
      byClass.put(dispatcher.getSampleClass(), e);
      userIndex++;
    }
    return e.getIndex();
  }
  
  public int getTag(SampleDispatcher d) throws IOException {
      return getTag(d.getSampleClass());
  }

  public int getTag(Class<? extends SampleType> sample) throws IOException {
    Entry e = byClass.get(sample);
    if (e == null) {
      throw new IOException("'" + 
			    sample.getSimpleName() + 
			    "' is not registered");
    }
    return e.index;
  }

  public boolean contains(Class<? extends SampleType> sample) {
    return byClass.containsKey(sample);
  }

}
