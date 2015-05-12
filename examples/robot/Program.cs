using se.lth.control.labcomm2014;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace RobotCtrl
{
    class Specification
    {

        public static string IP_ADDRESS = "127.0.0.1";
        public static int PORT = 51337;

        static void Main(string[] args)
        {
            jointtarget val = new jointtarget { robax = new jointtarget.struct_robax(), extax = new jointtarget.struct_extax() };
            TcpClient client = new TcpClient();
            IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse(Specification.IP_ADDRESS), Specification.PORT);
            try
            {
                client.Connect(serverEndPoint);
                Encoder enc = new EncoderChannel(client.GetStream(), true);
                jointtarget.register(enc);
                jointtarget.encode(enc, val);
                for (int i = 0; i < 10; i++)
                {
                    val.robax.rax_2 += 2;
                    jointtarget.encode(enc, val);
                }
                for (int i = 0; i < 20; i++)
                {
                    val.robax.rax_2 -= 2;
                    jointtarget.encode(enc, val);
                }
                for (int i = 0; i < 10; i++)
                {
                    val.robax.rax_2 += 2;
                    jointtarget.encode(enc, val);
                }
            }
            finally
            {
                client.Close();
            }
        }
    }
}
