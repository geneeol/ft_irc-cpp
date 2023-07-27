#include "Validator.hpp"

// TODO EnvManager 고치기
#define PASSWORD "1234"

bool Validator::Visit(CapRequest *capRequest) const
{
}

bool Validator::Visit(InviteRequest *inviteRequest) const
{
}

bool Validator::Visit(JoinRequest *joinRequest) const
{
}

bool Validator::Visit(KickRequest *kickRequest) const
{
}

bool Validator::Visit(ModeRequest *modeRequest) const
{
}

bool Validator::Visit(NickRequest *nickRequest) const
{
}

bool Validator::Visit(PartRequest *partRequest) const
{
}

// Password 틀릴 경우 그냥 바로 Closing Link - 원래는 보관했다가 register 할 때
// 검증
bool Validator::Visit(PassRequest *passRequest) const
{
    Client *client = passRequest->GetClient();

    if (client->EnteredPassword() && client->EnteredUserInfo() && client->EnteredNickName())
    {
        std::string errorMessage;

        if (client->EnteredNickName())
            errorMessage = BuildAlreadyRegisteredMsg(client->GetNickName());
        else
            errorMessage = BuildAlreadyRegisteredMsg();

        client->InsertResponse(errorMessage);
        return true;
    }

    if (client->EnteredPassword())
        return true;

    if (passRequest->GetPassword() != PASSWORD)
    {
        std::string errorMessage;

        if (client->EnteredUserInfo())
            errorMessage = BuildAccessDeniedMsg(client->GetUserName(), client->GetHostName());
        else
            errorMessage = BuildAccessDeniedMsg(); // TODO 사용자 hostname 은 가져와서 넣기

        client->InsertResponse(errorMessage);
        return false;
    }

    return true;
}

bool Validator::Visit(PingRequest *pingRequest) const
{
}

bool Validator::Visit(PrivmsgRequest *privmsgRequest) const
{
}

bool Validator::Visit(QuitRequest *quitRequest) const
{
}

bool Validator::Visit(TopicRequest *topicRequest) const
{
}

bool Validator::Visit(UserRequest *userRequest) const
{
    Client *client = userRequest->GetClient();

    if (client->EnteredUserInfo())
    {
        std::string errorMessage;

        if (client->EnteredNickName())
            errorMessage = BuildAlreadyRegisteredMsg(client->GetNickName());
        else
            errorMessage = BuildAlreadyRegisteredMsg();

        client->InsertResponse(errorMessage);
        return false;
    }

    if (client->EnteredNickName() && !client->EnteredPassword())
    {
        std::string errorMessage;

        errorMessage = BuildAccessDeniedMsg(userRequest->GetUserName(), userRequest->GetHostName());

        client->InsertResponse(errorMessage);
        return false;
    }

    return true;
}

std::string Validator::BuildAlreadyRegisteredMsg(const std::string &nickName)
{
    std::stringstream ErrorMessage;

    // TODO EnvManager에서 서버 이름 가져오기
    ErrorMessage << ":irc.local 462 " << nickName << ":You may not reregister";

    return ErrorMessage.str();
}

// TODO 원래는 GetHostName()이 아니라 아이피 주소로 출력됨 방법 찾기
std::string Validator::BuildAccessDeniedMsg(const std::string &userName, const std::string &hostName)
{
    std::stringstream ErrorMessage;

    ErrorMessage << "Error :Closing link: (" << userName << "@" << hostName << ") [Access denied by configuration]";

    return ErrorMessage.str();
}