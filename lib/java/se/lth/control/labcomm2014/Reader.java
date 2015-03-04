package se.lth.control.labcomm2014;

public interface Reader {

  public void handle(byte[] data, int begin, int end);

}
