using FluentScheduler;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleSerialMail
{
    class Program
    {
        static void Main(string[] args)
        {
            TaskManager.Initialize(new MailRegistry());

            Console.Write("Escreva para parar");
            Console.ReadKey();

        }
    }


    class MailRegistry : Registry
    {
        public MailRegistry()
        {
            Schedule(() =>
            {
                try
                {
                    MailService.Loop();
                }
                catch { }
            })
            .ToRunNow()
            .AndEvery(5)
            .Seconds();
        }
    }
}
