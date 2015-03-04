using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using se.lth.control.labcomm2014;

namespace user_types
{
    public class Encoder
    {
        private EncoderChannel enc;
        public Encoder(Stream stream)
        {
            enc = new EncoderChannel(stream);
            twoLines.register(enc);
        }

        public void doEncode()
        {
            twoLines x = new twoLines();
            line l1 = new line();
            point p11 = new point();
            coord c11x = new coord();
            coord c11y = new coord();
            c11x.val = 11;
            c11y.val = 99;
            p11.x = c11x;
            p11.y = c11y;

            l1.start = p11;

            point p12 = new point();
            coord c12x = new coord();
            coord c12y = new coord();
            c12x.val = 22;
            c12y.val = 88;
            p12.x = c12x;
            p12.y = c12y;

            l1.end = p12;

            line l2 = new line();
            point p21 = new point();
            coord c21x = new coord();
            coord c21y = new coord();
            c21x.val = 17;
            c21y.val = 42;
            p21.x = c21x;
            p21.y = c21y;

            l2.start = p21;

            point p22 = new point();
            coord c22x = new coord();
            coord c22y = new coord();
            c22x.val = 13;
            c22y.val = 37;
            p22.x = c22x;
            p22.y = c22y;

            l2.end = p22;

            foo f = new foo();
            f.a = 10;
            f.b = 20;
            f.c = false;

            x.l1 = l1;
            x.l2 = l2;
            x.f = f;

            Console.WriteLine("Encoding theTwoLines");
            twoLines.encode(enc, x);
        }

        static void Main(string[] args)
        {
            FileStream stream = new FileStream(args[0],FileMode.Create);
            Encoder example = new Encoder(stream);
            example.doEncode();
            stream.Close();
        }
    }
}
