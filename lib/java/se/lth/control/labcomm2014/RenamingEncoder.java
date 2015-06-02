package se.lth.control.labcomm2014;

import java.io.IOException;
import java.util.HashMap;

public class RenamingEncoder extends WrappingEncoder {

  public interface Rename {
    String rename(String name);   
  }

  private RenamingRegistry registry;
  private Rename rename;
  private HashMap<SampleDispatcher, SampleDispatcher> alias;

  public RenamingEncoder(Encoder encoder,
                         RenamingRegistry registry,
                         Rename rename) throws IOException {
    super(encoder);
    this.registry = registry;
    this.rename = rename;
    alias = new HashMap<SampleDispatcher, SampleDispatcher>();
  }

  private synchronized SampleDispatcher add(SampleDispatcher identity) {
    SampleDispatcher renamed = alias.get(identity);
    if (renamed == null) {
      renamed = registry.add(identity, rename.rename(identity.getName()));
      alias.put(identity, renamed);
    }
    return renamed;
  }

  private synchronized SampleDispatcher get(SampleDispatcher identity) {
    return alias.get(identity);
  }

  public void register(SampleDispatcher identity) throws IOException {
    super.register(add(identity));
  }
  
  public void registerSampleRef(SampleDispatcher identity) throws IOException {
    super.registerSampleRef(add(identity));
  }
  
  public void begin(SampleDispatcher identity) throws IOException {
    super.begin(get(identity));
  }
  
  public void end(SampleDispatcher identity) throws IOException {
    super.end(get(identity));
  }

} 
