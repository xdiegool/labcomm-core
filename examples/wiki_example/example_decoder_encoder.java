import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

import se.lth.control.labcomm.LabCommDecoderChannel;
import se.lth.control.labcomm.LabCommEncoderChannel;

public class example_decoder_encoder 
  implements data.Handler, log_message.Handler 
{

  LabCommDecoderChannel decoder;
  LabCommEncoderChannel encoder;

  public example_decoder_encoder(InputStream in, OutputStream out) 
    throws Exception 
  {
    decoder = new LabCommDecoderChannel(in);
    log_message.register(decoder, this);
    data.register(decoder, this);

    encoder = new LabCommEncoderChannel(out);
    log_message.register(encoder);
    data.register(encoder);
 
    try {
      decoder.run();
    } catch (java.io.EOFException e) {
    }
  }

  public void handle_data(float v) throws java.io.IOException {
    System.out.println("Got data");
    data.encode(encoder, v);
  }

  public void handle_log_message(log_message v) throws java.io.IOException {
    System.out.println("Got log_message");
    log_message.encode(encoder, v);
  }


  public static void main(String[] arg) throws Exception {
    example_decoder_encoder example = new example_decoder_encoder(
      new FileInputStream(new File(arg[0])),
      new FileOutputStream(new File(arg[1])));


    
  }

}

