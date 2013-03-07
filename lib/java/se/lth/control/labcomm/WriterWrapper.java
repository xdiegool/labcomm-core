package se.lth.control.labcomm;

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
