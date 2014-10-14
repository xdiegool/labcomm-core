package se.lth.control.labcomm;

public interface Reader {

  public void handle(byte[] data, int begin, int end);

}
