package se.lth.control.labcomm2014;

import java.io.IOException;
import java.util.HashMap;

public class RenamingDecoder extends WrappingDecoder {

  public interface Rename {
    String rename(String name);   
  }

  private RenamingRegistry registry;
  private Rename rename;
    
  public RenamingDecoder(Decoder decoder,
                         RenamingRegistry registry,
                         Rename rename) throws IOException {
    super(decoder);
    this.registry = registry;
    this.rename = rename;
  }

  public void register(SampleDispatcher dispatcher, 
                       SampleHandler handler) throws IOException {
    super.register(registry.add(
                     dispatcher,
                     rename.rename(dispatcher.getName())),
                   handler);
  }

  public void registerSampleRef(SampleDispatcher dispatcher) throws IOException {
    super.registerSampleRef(registry.add(
                              dispatcher,
                              rename.rename(dispatcher.getName())));
  }

} 
