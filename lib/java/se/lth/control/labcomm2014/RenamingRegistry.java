package se.lth.control.labcomm2014;

import java.util.HashMap;

public class RenamingRegistry {

  public static class Dispatcher implements SampleDispatcher {
    
    private final SampleDispatcher dispatcher;
    private final String name;
    
    public Dispatcher(SampleDispatcher dispatcher,
                      String name) {
      this.dispatcher = dispatcher;
      this.name = name;
    }
    
    public String getName() {
      return name;
    }
    
    public byte[] getSignature() {
      return dispatcher.getSignature();
    }
    
    public void decodeAndHandle(Decoder decoder,
                                SampleHandler handler) throws Exception {
      dispatcher.decodeAndHandle(decoder, handler);
    }
    
    public boolean equals(Object obj) {
      if (! (obj instanceof Dispatcher)) {
        return false;
      } else {
        Dispatcher other = (Dispatcher)obj;
        return (dispatcher == other.dispatcher &&
                name.equals(other.name));
      }
    }
    
    public int hashCode() {
      return dispatcher.hashCode() ^ name.hashCode();
    }
    
  }

  private HashMap<Dispatcher, Dispatcher> registry;

  public RenamingRegistry() {
    registry = new HashMap<Dispatcher, Dispatcher>();
  }
  
  public synchronized SampleDispatcher add(SampleDispatcher dispatcher,
                                           String newName) {
    Dispatcher tmp = new Dispatcher(dispatcher, newName);
    Dispatcher result = registry.get(tmp);
    if (result == null) {
      registry.put(tmp, tmp);
      result = tmp;
    }
    return result;
  }

}
