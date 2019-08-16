#pragma once

#ifndef CODE_STATE_H
#define CODE_STATE_H

#include <string>

void pushCodeState(std::string_view state);

void popCodeState();

#endif