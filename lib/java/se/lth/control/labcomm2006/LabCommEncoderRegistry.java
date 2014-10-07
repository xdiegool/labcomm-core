package se.lth.control.labcomm2006;

import java.io.IOException;
import java.util.HashMap;

public class LabCommEncoderRegistry {

  public static class Entry {
    
    private LabCommDispatcher dispatcher;
    private int index;

    public Entry(LabCommDispatcher dispatcher, int index) {
      this.dispatcher = dispatcher;
      this.index = index;
    }

    public LabCommDispatcher getDispatcher() {
      return dispatcher;
    }

    public int getIndex() {
      return index;
    }

  }

  private int userIndex = LabComm.FIRST_USER_INDEX;
  private HashMap<Class, Entry> byClass;

  public LabCommEncoderRegistry() {
    byClass = new HashMap<Class, Entry>();
  }

  public synchronized int add(LabCommDispatcher dispatcher) {
    Entry e = byClass.get(dispatcher.getSampleClass());
    if (e == null) {
      e = new Entry(dispatcher, userIndex);
      byClass.put(dispatcher.getSampleClass(), e);
      userIndex++;
    }
    return e.getIndex();
  }
  
  public int getTag(Class<? extends LabCommSample> sample) throws IOException {
    Entry e = byClass.get(sample);
    if (e == null) {
      throw new IOException("'" + 
			    sample.getSimpleName() + 
			    "' is not registered");
    }
    return e.index;
  }

}