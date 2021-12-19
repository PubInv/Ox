/*
Public Invention's Ox Project is an open source hardware design for an oxygen
concentrator for use by field hospitals around the world. This team aims to
design an oxygen concentrator that can be manufactured locally while overcoming
challenges posed by human resources, hospital location (geographically),
infrastructure and logistics; in addition, this project attempts the minimum
documentation expected of their design for international approval whilst
tackling regulatory requirements for medical devices. Copyright (C) 2021
Robert Read, Ben Coombs, and Darío Hereñú.

This program includes free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

See the GNU Affero General Public License for more details.
You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "collections/list.h"

namespace OxCore {

enum class ErrorMode {
    Log,
    StdOut
};

enum class ErrorType {
    Null = 0,
    LessThanZero,
    OutOfBounds
};

enum class ErrorLevel {
    Info,
    Debug,
    Warning,
    Error,
    Critical
};

const char *ErrorMessage[] = {
    "Null",
    "LessThanZero",
    "OutOfBounds"
};

struct Error {
    ErrorLevel level;
    ErrorType type;
};

// This is the error string to output from the above Error enum
//static const char *ErrorString[] = { "Null", "LessThanZero", "OutOfBounds" };

ErrorMode errorMode;
void InitErrorHandler(ErrorMode mode);
void HandleError(Error error);

class ErrorHandler {
    private:
        static int count;
        OxCollections::List<Error, 10> _errors;
    public:
        void Log(ErrorLevel level, ErrorType type) {
            Error error = {level, type};
            _errors.add(error);
        }

};

}

#endif
