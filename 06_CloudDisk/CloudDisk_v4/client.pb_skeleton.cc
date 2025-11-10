#include "user_service.srpc.h"
#include "workflow/WFFacilities.h"

using namespace srpc;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

static void signup_done(::user_service::SignupResponse *response, srpc::RPCContext *context)
{
}

static void signin_done(::user_service::SignupResponse *response, srpc::RPCContext *context)
{
}

static void getuserinfo_done(::user_service::GetUserInfoResponse *response, srpc::RPCContext *context)
{
}

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	const char *ip = "127.0.0.1";
	unsigned short port = 1412;

	::user_service::UserService::SRPCClient client(ip, port);

	// example for RPC method call
	::user_service::SignupRequest signup_req;
	//signup_req.set_message("Hello, srpc!");
	client.Signup(&signup_req, signup_done);

	wait_group.wait();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
