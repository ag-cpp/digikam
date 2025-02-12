/*****************************************************************
|
|   Neptune - Messaging System
|
| Copyright (c) 2002-2008, Axiomatic Systems, LLC.
| All rights reserved.
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions are met:
|     * Redistributions of source code must retain the above copyright
|       notice, this list of conditions and the following disclaimer.
|     * Redistributions in binary form must reproduce the above copyright
|       notice, this list of conditions and the following disclaimer in the
|       documentation and/or other materials provided with the distribution.
|     * Neither the name of Axiomatic Systems nor the
|       names of its contributors may be used to endorse or promote products
|       derived from this software without specific prior written permission.
|
| THIS SOFTWARE IS PROVIDED BY AXIOMATIC SYSTEMS ''AS IS'' AND ANY
| EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
| WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
| DISCLAIMED. IN NO EVENT SHALL AXIOMATIC SYSTEMS BE LIABLE FOR ANY
| DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
| (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
| ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
| (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
| SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
 ****************************************************************/

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "NptMessaging.h"

/*----------------------------------------------------------------------
|   globals
+---------------------------------------------------------------------*/
NPT_DEFINE_DYNAMIC_CAST_ANCHOR(NPT_MessageHandler)
const NPT_Message::Type NPT_Message::MessageType = "Generic Message";

/*----------------------------------------------------------------------
|   NPT_MessageHandler::HandleMessage
+---------------------------------------------------------------------*/
NPT_Result
NPT_MessageHandler::HandleMessage(NPT_Message* message)
{
    return message->Dispatch(this);
}

/*----------------------------------------------------------------------
|   NPT_MessageHandlerProxy::NPT_MessageHandlerProxy
+---------------------------------------------------------------------*/
NPT_DEFINE_DYNAMIC_CAST_ANCHOR(NPT_MessageHandlerProxy)
NPT_MessageHandlerProxy::NPT_MessageHandlerProxy(NPT_MessageHandler* handler) :
    m_Handler(handler),
    m_ReferenceCount(1)
{}

/*----------------------------------------------------------------------
|   NPT_MessageHandlerProxy::NPT_MessageHandlerProxy
+---------------------------------------------------------------------*/
NPT_MessageHandlerProxy::~NPT_MessageHandlerProxy()
{
}

/*----------------------------------------------------------------------
|   NPT_MessageHandlerProxy::OnMessage
+---------------------------------------------------------------------*/
void
NPT_MessageHandlerProxy::OnMessage(NPT_Message* message)
{
    m_Lock.Lock();
    if (m_Handler) m_Handler->OnMessage(message);
    m_Lock.Unlock();
}

/*----------------------------------------------------------------------
|   NPT_MessageHandlerProxy::HandleMessage
+---------------------------------------------------------------------*/
NPT_Result
NPT_MessageHandlerProxy::HandleMessage(NPT_Message* message)
{
    NPT_Result result = NPT_SUCCESS;
    m_Lock.Lock();
    if (m_Handler) result = m_Handler->HandleMessage(message);
    m_Lock.Unlock();

    return result;
}

/*----------------------------------------------------------------------
|   NPT_MessageHandlerProxy::DetachHandler
+---------------------------------------------------------------------*/
void
NPT_MessageHandlerProxy::DetachHandler()
{
    m_Lock.Lock();
    m_Handler = NULL;
    m_Lock.Unlock();
}

/*----------------------------------------------------------------------
|   NPT_MessageHandlerProxy::AddReference
+---------------------------------------------------------------------*/
void
NPT_MessageHandlerProxy::AddReference()
{
    m_Lock.Lock();
    ++m_ReferenceCount;
    m_Lock.Unlock();
}

/*----------------------------------------------------------------------
|   NPT_MessageHandlerProxy::Release
+---------------------------------------------------------------------*/
void
NPT_MessageHandlerProxy::Release()
{
    m_Lock.Lock();
    bool must_delete = (--m_ReferenceCount == 0);
    m_Lock.Unlock();

    if (must_delete) delete this;
}
