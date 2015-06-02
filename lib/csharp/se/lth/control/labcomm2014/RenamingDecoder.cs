namespace se.lth.control.labcomm2014 {

  using System;

  public class RenamingDecoder : WrappingDecoder {

    private RenamingRegistry registry;
    private Func<String,String> rename;
    
    public RenamingDecoder(Decoder decoder,
                           RenamingRegistry registry,
                           Func<String,String> rename)
    : base(decoder) {
      this.registry = registry;
      this.rename = rename;
    }

    public override void register(SampleDispatcher dispatcher, 
			 SampleHandler handler) {
      base.register(registry.add(
                      dispatcher,
                      rename(dispatcher.getName())),
                    handler);
    }

    public override void registerSampleRef(SampleDispatcher dispatcher) {
      base.registerSampleRef(registry.add(
                               dispatcher,
                               rename(dispatcher.getName())));
    }


  }

} 
