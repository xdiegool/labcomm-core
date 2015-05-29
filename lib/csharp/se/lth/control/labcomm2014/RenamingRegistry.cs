namespace se.lth.control.labcomm2014 {

  using System;
  using System.Collections.Generic;

  public class RenamingRegistry {

    public class Dispatcher : SampleDispatcher, IEquatable<Dispatcher> {

      private SampleDispatcher dispatcher;
      private String name;
      
      public Dispatcher(SampleDispatcher dispatcher,
                        String name) {
        this.dispatcher = dispatcher;
        this.name = name;
      }

      public SampleDispatcher getSampleIdentity() {
        return this;
      }
    
      public String getName() {
        return name;
      }
    
      public byte[] getSignature() {
        return dispatcher.getSignature();
      }

      public void decodeAndHandle(Decoder decoder,
  			   SampleHandler handler) {
        dispatcher.decodeAndHandle(decoder, handler);
      }

      public bool Equals(Dispatcher obj) {
        Dispatcher other = obj as Dispatcher;
        return (other != null &&
                dispatcher == other.dispatcher &&
                name.Equals(other.name));
      }

      public override int GetHashCode() {
        return dispatcher.GetHashCode() ^ name.GetHashCode();
      }
      
      public override string ToString() {
        return "RenamingRegistry.Dispatcher(" + name + ")";
      }

    }

    private Dictionary<Dispatcher, Dispatcher> registry;

    public RenamingRegistry() {
      registry = new Dictionary<Dispatcher, Dispatcher>();
    }
    
    public SampleDispatcher add(SampleDispatcher dispatcher,
                                String newName) {
      Dispatcher result;
      Dispatcher tmp = new Dispatcher(dispatcher, newName);
      lock(this) {
        registry.TryGetValue(tmp, out result);
	if (result == null) {
          registry.Add(tmp, tmp);
          result = tmp;
        }
      }
      return result;
    }

  }

}
