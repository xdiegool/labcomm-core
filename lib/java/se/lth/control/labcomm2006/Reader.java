package se.lth.control.labcomm2006;

public interface Reader {

  public void handle(byte[] data, int begin, int end);

}
