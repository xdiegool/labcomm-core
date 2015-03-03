namespace se.lth.control.labcomm2006 {

  using System;
  using System.Collections.Generic;

  public class DecoderRegistry {

    public class Entry {
    
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

      public SampleDispatcher getSampleDispatcher() {
	return dispatcher;
      }

      public void setSampleDispatcher(SampleDispatcher dispatcher) {
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

      public String getSignature() {
	return BitConverter.ToString(signature);
      }

      public void setIndex(int index) {
	if (this.index != 0 && this.index != index) {
	  throw new Exception("Index mismatch " + 
			      this.index + " != " + index);
	}
	this.index = index;
      }

      public bool matchName(String name) {
	return this.name.Equals(name);
      }

      public bool matchSignature(byte[] signature) {
	bool result = this.signature.Length == signature.Length;
	for (int i = 0 ; result && i < signature.Length ; i++) {
	  result = this.signature[i] == signature[i];
	}
	return result;
      }

      public bool match(String name, byte[] signature) {
	return matchName(name) && matchSignature(signature);
      }

      public void check(String name, byte[] signature) {
	if (!matchName(name)) {
	  throw new Exception("Name mismatch '" + 
				this.name + "' != '" + name + "'");
	} 
	if (!matchSignature(signature)) {
	  throw new Exception("Signature mismatch");
	} 
      }
    }

    private Dictionary<Type, Entry> byClass;
    private Dictionary<int, Entry> byIndex;

    public DecoderRegistry() {
      byClass = new Dictionary<Type, Entry>();
      byIndex = new Dictionary<int, Entry>();
    }

    public void add(SampleDispatcher dispatcher,
		    SampleHandler handler) {
      lock(this) {
	Entry e;
	byClass.TryGetValue(dispatcher.getSampleClass(), out e);
	if (e != null) {
	  e.check(dispatcher.getName(), dispatcher.getSignature());
	  e.setHandler(handler);
	} else {
	  foreach (Entry e2 in byIndex.Values) {
	    if (e2.match(dispatcher.getName(), dispatcher.getSignature())) {
	      e2.setSampleDispatcher(dispatcher);
	      e2.setHandler(handler);
	      e = e2;
	      break;
	    }
	  }
	  if (e == null) {
	    e = new Entry(dispatcher, handler);
	    byClass.Add(dispatcher.getSampleClass(), e);
	  }
	}
      }
    }

    public void add(int index, 
		    String name,
		    byte[] signature) {
      lock(this) {
	Entry e;
	byIndex.TryGetValue(index, out e);
	if (e != null) {
	  e.check(name, signature);
	} else {
	  foreach (Entry e2 in byClass.Values) {
	    if (e2.match(name, signature)) {
	      e2.setIndex(index);
	      e = e2;
	      break;
	    }
	  }
	  if (e == null) {
	    e = new Entry(index, name, signature);
	  }
	  byIndex.Add(index, e);
	}
      }
    }
    
    public Entry get(int index) {
      lock(this) {
	Entry result;
	byIndex.TryGetValue(index, out result);
	return result;
      }
    }

  }

}
