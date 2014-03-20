using Limilabs.Client.IMAP;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO.Ports;

namespace MailSerialService
{
    public static class MailService
    {
        private static string eMailFromUser;
        private static string eMailFromPassWord;
        private static string clientUrl;
        private static Imap client;

        private static string portName;
        private static int baudRate;
        private static SerialPort serialport;

        private static void Connect()
        {
            client = new Imap();
            if (!client.Connected) // Duump
            {
                client.Connect(clientUrl, 993, true);   // or ConnectSSL for SSL
                client.Login(eMailFromUser, eMailFromPassWord);
            }

            serialport = new SerialPort(portName, baudRate);
            if (!serialport.IsOpen)
                serialport.Open();
        }

        public static void DisConnect()
        {
            if (client != null)
            {
                if (client.Connected)
                    client.Close();
                client.Dispose();
            }

            if (serialport != null)
            {
                if (serialport.IsOpen)
                    serialport.Close();
                serialport.Dispose();
            }
        }

        static MailService()
        {
            eMailFromUser = ConfigurationManager.AppSettings["eMailFromUser"].ToString();
            eMailFromPassWord = ConfigurationManager.AppSettings["eMailFromPassWord"].ToString();
            clientUrl = ConfigurationManager.AppSettings["clientUrl"].ToString();

            portName = ConfigurationManager.AppSettings["sPortName"].ToString();
            baudRate = Convert.ToInt32(ConfigurationManager.AppSettings["sBaudRate"]);
        }

        public static void Loop()
        {
            Connect();

            client.SelectInbox();
            List<long> uids = client.Search(Flag.Unseen);

            bool newMessages = uids.Count > 0;
            string msg = newMessages ? "l" : "d";

            serialport.Write(msg);

            DisConnect();
        }
    }
}
