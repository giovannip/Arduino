using FluentScheduler;
using System.ServiceProcess;

namespace MailSerialService
{
    public partial class MService : ServiceBase
    {
        public MService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            #region FluentScheduler Start
            TaskManager.Initialize(new MailRegistry());
            #endregion
        }

        protected override void OnStop()
        {
            MailService.DisConnect();
        }
    }

    public class MailRegistry : Registry
    {
        public MailRegistry()
        {
            Schedule(() =>
            {
                MailService.Loop();
            })
            .ToRunNow()
            .AndEvery(30)
            .Seconds();
        }
    }
}
