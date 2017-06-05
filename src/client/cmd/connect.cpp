/*
 * Copyright (C) 2017 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Alberto Aguirre <alberto.aguirre@canonical.com>
 *
 */

#include "connect.h"

#ifdef MULTIPASS_PLATFORM_POSIX
#include <unistd.h>
#endif

#include <sstream>
#include <vector>

namespace mp = multipass;
namespace cmd = multipass::cmd;
using RpcMethod = mp::Rpc::Stub;

namespace {
auto parse_exec_line(std::string exec_line)
{
    std::istringstream iss(exec_line);

    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>{}};

    return tokens;
}

auto to_argv(const std::vector<std::string>& v)
{
    std::vector<char*> result;
    for (const auto& s : v)
        result.push_back(const_cast<char*>(s.c_str()));
    result.push_back(nullptr);
    return result;
}

auto execute_process(std::string exec_line)
{
    auto parsed_cmd = parse_exec_line(exec_line);
    auto cmd = to_argv(parsed_cmd);
#ifdef MULTIPASS_PLATFORM_POSIX
    return execvp(cmd[0], cmd.data());
#else
    return EXIT_SUCCESS;
#endif
}
}

int cmd::Connect::run()
{
    auto on_success = [this](mp::ConnectReply& reply) {
        if (reply.exec_line().empty())
        {
            cout << "received connect reply\n";
            return EXIT_SUCCESS;
        }
        else
        {
            return execute_process(reply.exec_line());
        }
    };

    auto on_failure = [this](grpc::Status& status) {
        cerr << "connect failed: " << status.error_message() << "\n";
        return EXIT_FAILURE;
    };

    mp::ConnectRequest request;
    return dispatch(&RpcMethod::connect, request, on_success, on_failure);
}

std::string cmd::Connect::name() const { return "connect"; }
