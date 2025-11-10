#include "user_service.srpc.h"
#include "workflow/WFFacilities.h"

using namespace srpc;

static WFFacilities::WaitGroup wait_group(1);

void sig_handler(int signo)
{
	wait_group.done();
}

class UserServiceServiceImpl : public ::user_service::UserService::Service
{
public:

	void Signup(::user_service::SignupRequest *request, ::user_service::SignupResponse *response, srpc::RPCContext *ctx) override
	{
		// TODO: fill server logic here
	}

	void Signin(::user_service::SigninRequest *request, ::user_service::SigninResponse *response, srpc::RPCContext *ctx) override
	{
		// TODO: fill server logic here
	}

	void GetUserInfo(::user_service::GetUserInfoRequest *request, ::user_service::GetUserInfoResponse *response, srpc::RPCContext *ctx) override
	{
		// TODO: fill server logic here
	}
};

int main()
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	unsigned short port = 1412;
	SRPCServer server;

	UserServiceServiceImpl userservice_impl;
	server.add_service(&userservice_impl);

	server.start(port);
	wait_group.wait();
	server.stop();
	google::protobuf::ShutdownProtobufLibrary();
	return 0;
}
