namespace se.lth.control.labcomm2014 {

  using System;
  using System.Collections.Generic;

  public class RenamingEncoder : WrappingEncoder {

    private Encoder encoder;
    private RenamingRegistry registry;
    private Func<String,String> rename;
    private Dictionary<SampleDispatcher, SampleDispatcher> alias;

    public RenamingEncoder(Encoder encoder,
                           RenamingRegistry registry,
                           Func<String,String> rename)
    : base(encoder) {
      this.encoder = encoder;
      this.registry = registry;
      this.rename = rename;
      alias = new Dictionary<SampleDispatcher, SampleDispatcher>();
    }

    private SampleDispatcher add(SampleDispatcher identity) {
      SampleDispatcher renamed;
      lock(this) {
        if (! alias.TryGetValue(identity, out renamed)) {
          renamed = registry.add(identity, rename(identity.getName()));
          alias.Add(identity, renamed);
        }
      }
      get(identity);
      return renamed;
    }

    private SampleDispatcher get(SampleDispatcher identity) {
      SampleDispatcher renamed;
      lock(this) {
        alias.TryGetValue(identity, out renamed);
      }
      return renamed;
    }
    public override void register(SampleDispatcher identity) {
      encoder.register(add(identity));
    }

    public override void registerSampleRef(SampleDispatcher identity) {
      encoder.registerSampleRef(add(identity));
    }

    public override void begin(SampleDispatcher identity) {
      base.begin(get(identity));
    }

    public override void end(SampleDispatcher identity) {
      base.end(get(identity));
    }

  }

} 
