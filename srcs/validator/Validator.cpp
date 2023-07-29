/*
 * PASS, NICK, USER, CAP 빼고는 다 Registered 안되어 있으면 NotRegisteredMsg
 *
 * <Validator 구현 완료 명령어>
 *
 * PASS
 * NICK
 * USER
 * TOPIC
 *
 *
 * */

#include "Validator.hpp"

bool Validator::Visit(CapRequest *capRequest) const {}

bool Validator::Visit(InviteRequest *inviteRequest) const
{
    Client *client = inviteRequest->GetClient();

    if (!client->HasRegistered())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildNotRegisteredMsg("INVITE", client->GetNickName());
        else
            errorMessage = BuildNotRegisteredMsg("INVITE");

        client->InsertResponse(errorMessage);

        return false;
    }
}

bool Validator::Visit(JoinRequest *joinRequest) const
{
    Client *client = joinRequest->GetClient();

    if (!client->HasRegistered())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildNotRegisteredMsg("JOIN", client->GetNickName());
        else
            errorMessage = BuildNotRegisteredMsg("JOIN");

        client->InsertResponse(errorMessage);

        return false;
    }
}

bool Validator::Visit(KickRequest *kickRequest) const
{
    Client *client = kickRequest->GetClient();

    if (!client->HasRegistered())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildNotRegisteredMsg("KICK", client->GetNickName());
        else
            errorMessage = BuildNotRegisteredMsg("KICK");

        client->InsertResponse(errorMessage);

        return false;
    }
}

bool Validator::Visit(ModeRequest *modeRequest) const
{
    Client *client = modeRequest->GetClient();

    if (!client->HasRegistered())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildNotRegisteredMsg("MODE", client->GetNickName());
        else
            errorMessage = BuildNotRegisteredMsg("MODE");

        client->InsertResponse(errorMessage);

        return false;
    }
}

// NICK Command 경우의 수 검증 완료
bool Validator::Visit(NickRequest *nickRequest) const
{
    ClientRepository *clientRepository = ClientRepository::GetInstance();
    Client *client = nickRequest->GetClient();

    // 이미 해당 닉네임을 사용하는 클라이언트가 존재하는 경우
    if (clientRepository->FindByNickname(nickRequest->GetNickName()) != NULL)
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildNickNameInUseMsg(nickRequest->GetNickName(), client->GetNickName());
        else
            errorMessage = BuildNickNameInUseMsg(nickRequest->GetNickName());

        client->InsertResponse(errorMessage);
        return false;
    }

    // Password를 입력하지 않았는데 register를 시도하는 경우
    if (client->HasEnteredUserInfo() && !client->HasEnteredPassword())
    {
        std::string errorMessage;

        errorMessage = BuildAccessDeniedMsg(client->GetUserName(), client->GetHostName());

        client->InsertResponse(errorMessage);
        return false;
    }

    return true;
}

bool Validator::Visit(PartRequest *partRequest) const
{
    Client *client = partRequest->GetClient();

    if (!client->HasRegistered())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildNotRegisteredMsg("PART", client->GetNickName());
        else
            errorMessage = BuildNotRegisteredMsg("PART");

        client->InsertResponse(errorMessage);

        return false;
    }
}

// User Command 경우의 수 검증 완료
bool Validator::Visit(PassRequest *passRequest) const
{
    Client *client = passRequest->GetClient();

    if (client->HasRegistered())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildAlreadyRegisteredMsg(client->GetNickName());
        else
            errorMessage = BuildAlreadyRegisteredMsg();

        client->InsertResponse(errorMessage);
        return false;
    }

    EnvManager *envManager = EnvManager::GetInstance();

    if (passRequest->GetPassword() != envManager->GetConnectionPassord())
    {
        std::string errorMessage;

        if (client->HasEnteredUserInfo())
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
    Client *client = pingRequest->GetClient();

    if (!client->HasRegistered())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildNotRegisteredMsg("PING", client->GetNickName());
        else
            errorMessage = BuildNotRegisteredMsg("PING");

        client->InsertResponse(errorMessage);

        return false;
    }
    // 파라미터가 부족한 경우는 Parser에서 처리

    // Error 종류 중에 ERR_NOORIGIN (409)는 처리할 필요 없는 듯?

    return true;
}

bool Validator::Visit(PrivmsgRequest *privmsgRequest) const
{
    Client *client = privmsgRequest->GetClient();

    if (!client->HasRegistered())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildNotRegisteredMsg("PRIVMSG", client->GetNickName());
        else
            errorMessage = BuildNotRegisteredMsg("PRIVMSG");

        client->InsertResponse(errorMessage);

        return false;
    }
}

bool Validator::Visit(QuitRequest *quitRequest) const {}

bool Validator::Visit(TopicRequest *topicRequest) const
{
    Client *client = topicRequest->GetClient();

    if (!client->HasRegistered())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildNotRegisteredMsg("TOPIC", client->GetNickName());
        else
            errorMessage = BuildNotRegisteredMsg("TOPIC");

        client->InsertResponse(errorMessage);

        return false;
    }

    ChannelRepository *channelRepository = ChannelRepository::GetInstance();
    Channel *channel = channelRepository->FindByName(topicRequest->GetChannelName());

    // 채널 없음
    if (!channel)
    {
        std::string errorMessage;

        errorMessage = BuildNoSuchChannelMsg(client->GetNickName(), topicRequest->GetChannelName());

        client->InsertResponse(errorMessage);

        return false;
    }

    // 채널에 속하지 않음
    if (!channel->CheckClientIsExist(client->GetNickName()))
    {
        std::string errorMessage;

        errorMessage = BuildNotOnChannelMsg(client->GetNickName(), topicRequest->GetChannelName());

        client->InsertResponse(errorMessage);

        return false;
    }

    // 채널이 ProtectedTopic 모드일때 권한 없음
    if (channel->IsProtectedTopicMode() && !channel->CheckClientIsOperator(client->GetNickName()))
    {
        std::string errorMessage;

        errorMessage = BuildNotChannelOperatorMsg(client->GetNickName(), topicRequest->GetChannelName());

        client->InsertResponse(errorMessage);

        return false;
    }

    return true;
}

// User Command 경우의 수 검증 완료
bool Validator::Visit(UserRequest *userRequest) const
{
    Client *client = userRequest->GetClient();

    if (client->HasEnteredUserInfo())
    {
        std::string errorMessage;

        if (client->HasEnteredNickName())
            errorMessage = BuildAlreadyRegisteredMsg(client->GetNickName());
        else
            errorMessage = BuildAlreadyRegisteredMsg();

        client->InsertResponse(errorMessage);
        return false;
    }

    if (client->HasEnteredNickName() && !client->HasEnteredPassword())
    {
        std::string errorMessage;

        // TODO hostname Ip로 알아오기
        errorMessage = BuildAccessDeniedMsg(userRequest->GetUserName(), userRequest->GetHostName());

        client->InsertResponse(errorMessage);
        return false;
    }

    return true;
}

std::string Validator::BuildAlreadyRegisteredMsg(const std::string &nickName)
{
    EnvManager *envManager = EnvManager::GetInstance();
    std::stringstream errorMessage;

    // TODO EnvManager에서 서버 이름 가져오기
    errorMessage << ":" << envManager->GetServerName() << " 462 " << nickName << ":You may not reregister";

    return errorMessage.str();
}

// TODO 원래는 GetHostName()이 아니라 아이피 주소로 출력됨 방법 찾기
// TODO AccessDenied 는 연결을 끊어야 함!!!!!
std::string Validator::BuildAccessDeniedMsg(const std::string &userName, const std::string &hostName)
{
    std::stringstream errorMessage;

    errorMessage << "Error :Closing link: (" << userName << "@" << hostName << ") [Access denied by configuration]";

    return errorMessage.str();
}

std::string Validator::BuildNickNameInUseMsg(const std::string &newNickName, const std::string &clientNickName)
{
    EnvManager *envManager = EnvManager::GetInstance();
    std::stringstream errorMessage;

    errorMessage << ":" << envManager->GetServerName() << " 433 " << clientNickName << " " << newNickName
                 << " :Nickname is already in use.";

    return errorMessage.str();
}

std::string Validator::BuildNotRegisteredMsg(const std::string &commandType, const std::string &nickName)
{
    EnvManager *envManager = EnvManager::GetInstance();
    std::stringstream errorMessage;

    errorMessage << ":" << envManager->GetServerName() << " 451 " << nickName << " " << commandType
                 << " :You have not registered.";

    return errorMessage.str();
}

std::string Validator::BuildNoSuchChannelMsg(const std::string &nickName, const std::string &channelName)
{
    EnvManager *envManager = EnvManager::GetInstance();
    std::stringstream errorMessage;

    errorMessage << ":" << envManager->GetServerName() << " 403 " << nickName << " " << channelName
                 << " :No such channel";

    return errorMessage.str();
}

std::string Validator::BuildNotOnChannelMsg(const std::string &nickName, const std::string &channelName)
{
    EnvManager *envManager = EnvManager::GetInstance();
    std::stringstream errorMessage;

    errorMessage << ":" << envManager->GetServerName() << " 442 " << nickName << " " << channelName
                 << " :You're not on that channel!";

    return errorMessage.str();
}

std::string Validator::BuildNotChannelOperatorMsg(const std::string &nickName, const std::string &channelName)
{
    EnvManager *envManager = EnvManager::GetInstance();
    std::stringstream errorMessage;

    errorMessage << ":" << envManager->GetServerName() << " 482 " << nickName << " " << channelName
                 << " :You do not have access to change the topic on this channel";

    return errorMessage.str();
}