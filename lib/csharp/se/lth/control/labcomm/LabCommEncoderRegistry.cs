namespace se.lth.control.labcomm {

  using System;
  using System.Collections.Generic;

  public class LabCommEncoderRegistry {

    public class Entry {
    
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
    private Dictionary<Type, Entry> byClass;

    public LabCommEncoderRegistry() {
      byClass = new Dictionary<Type, Entry>();
    }

    public int add(LabCommDispatcher dispatcher) {
      lock(this) {
	Entry e;
	byClass.TryGetValue(dispatcher.getSampleClass(), out e);
	if (e == null) {
	  e = new Entry(dispatcher, userIndex);
	  byClass.Add(dispatcher.getSampleClass(), e);
	  userIndex++;
	}
	return e.getIndex();
      }
    }
  
    public int getTag(Type sample) {
      lock(this) {
	Entry e;
	byClass.TryGetValue(sample, out e);
	if (e == null) {
	  throw new Exception("'" + 
			      sample.ToString() + 
			      "' is not registered");
	}
	return e.getIndex();
      }
    }

  }

}