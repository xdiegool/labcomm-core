namespace se.lth.control.labcomm2014 {

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
    private Dictionary<SampleDispatcher, Entry> byIdentity;

    public EncoderRegistry() {
      byIdentity = new Dictionary<SampleDispatcher, Entry>();
    }

    public int add(SampleDispatcher dispatcher) {
      lock(this) {
	Entry e;
	byIdentity.TryGetValue(dispatcher.getSampleIdentity(), out e);
	if (e == null) {
	  e = new Entry(dispatcher, userIndex);
	  byIdentity.Add(dispatcher.getSampleIdentity(), e);
	  userIndex++;
	}
	return e.getIndex();
      }
    }
  
    public int getTag(SampleDispatcher sample) {
      lock(this) {
	Entry e;
	byIdentity.TryGetValue(sample, out e);
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
