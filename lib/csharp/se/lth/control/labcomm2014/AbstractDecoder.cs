using System;

namespace se.lth.control.labcomm2014 {

  public interface AbstractDecoder {

    void runOne();

    void run();

    void register(SampleDispatcher dispatcher, 
		  SampleHandler handler);

    void registerSampleRef(SampleDispatcher dispatcher);

  }

} 
