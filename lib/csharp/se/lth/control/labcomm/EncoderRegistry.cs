namespace se.lth.control.labcomm {

  using System;
  using System.Collections.Generic;

  public class EncoderRegistry {

    public class Entry {
    
      private SampleDispatcher dispatcher;
      private int index;

      public Entry(SampleDispatcher dispatcher, int index) {
	this.dispatcher = dispatcher;
	this.index = index;
      }

      public SampleDispatcher getSampleDispatcher() {
	return dispatcher;
      }

      public int getIndex() {
	return index;
      }

    }

    private int userIndex = Constant.FIRST_USER_INDEX;
    private Dictionary<Type, Entry> byClass;

    public EncoderRegistry() {
      byClass = new Dictionary<Type, Entry>();
    }

    public int add(SampleDispatcher dispatcher) {
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