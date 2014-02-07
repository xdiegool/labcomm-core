package se.lth.control.labcomm2006;

import java.io.OutputStream;
import java.io.IOException;

class WriterWrapper implements LabCommWriter{

  private OutputStream os;

  public WriterWrapper(OutputStream os) {
    this.os = os;
  }

  public void write(byte[] data) throws IOException {
    os.write(data);
  }
}
