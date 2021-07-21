#include "pb/service/c2g_user.pb.h"

#define PROTOBUF_NAMESPACE_ID google::protobuf

namespace RPC {

class C2G_UserLoginService_Stub : public CMD::C2G_UserLoginService_Stub
{
    using CMD::C2G_UserLoginService_Stub::C2G_UserLoginService_Stub;

public:
    void C2G_UserLogin_Solved(CMD::C2G_UserLoginRes* response);
};

}   // namespace RPC

