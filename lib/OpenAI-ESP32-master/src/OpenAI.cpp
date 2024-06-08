/*
  ToDo:
    - Support FS::File as input
    - Look into supporting "stream" responses?
    - Thread-Safe API?
*/

#include "OpenAI.h"
#include "HTTPClient.h"

// ! HEAVILY MODIFIED FOR BCOMFY, REMOVED UNUSED FUNCTIONS AND CLASSES

// Macros for building the request
#define reqAddString(var,val) \
  if(cJSON_AddStringToObject(req, var, val) == NULL){ \
    cJSON_Delete(req); \
    log_e("cJSON_AddStringToObject failed!"); \
    return result; \
  }

#define reqAddNumber(var,val) \
  if(cJSON_AddNumberToObject(req, var, val) == NULL){ \
    cJSON_Delete(req); \
    log_e("cJSON_AddNumberToObject failed!"); \
    return result; \
  }

#define reqAddBool(var,val) \
  if(cJSON_AddBoolToObject(req, var, val) == NULL){ \
    cJSON_Delete(req); \
    log_e("cJSON_AddBoolToObject failed!"); \
    return result; \
  }

#define reqAddItem(var,val) \
  if(!cJSON_AddItemToObject(req, var, val)){ \
    cJSON_Delete(req); \
    cJSON_Delete(val); \
    log_e("cJSON_AddItemToObject failed!"); \
    return result; \
  }

static String getJsonError(cJSON * json){
  if(json == NULL){
    return String("cJSON_Parse failed!");
  }
  if(!cJSON_IsObject(json)){
    return String("Response is not an object! " + String(cJSON_Print(json)));
  }
  if(cJSON_HasObjectItem(json, "error")){
    cJSON * error = cJSON_GetObjectItem(json, "error");
    if(!cJSON_IsObject(error)){
      return String("Error is not an object! " + String(cJSON_Print(error)));
    }
    if(!cJSON_HasObjectItem(error, "message")){
      return String("Error does not contain message! " + String(cJSON_Print(error)));
    }
    cJSON * error_message = cJSON_GetObjectItem(error, "message");
    return String(cJSON_GetStringValue(error_message));
  }
  return String();
}

OpenAI_StringResponse::~OpenAI_StringResponse(){
  if(data != NULL){
    for (unsigned int i = 0; i < len; i++){
      free(data[i]);
    }
    free(data);
  }
  if(error_str != NULL){
    free(error_str);
  }
}

//
// OpenAI
//

OpenAI::OpenAI(const char *openai_api_key)
    : api_key(openai_api_key)
{

}

OpenAI::~OpenAI(){

}

String OpenAI::upload(String endpoint, String boundary, uint8_t * data, size_t len) {
  log_d("\"%s\": boundary=%s, len=%u", endpoint.c_str(), boundary.c_str(), len);
  HTTPClient http;
  http.setTimeout(20000);
  http.begin("https://api.openai.com/v1/" + endpoint);
  http.addHeader("Content-Type", "multipart/form-data; boundary="+boundary);
  http.addHeader("Authorization", "Bearer " + api_key);
  int httpCode = http.sendRequest("POST", data, len);
  if (httpCode != HTTP_CODE_OK) {
    log_e("HTTP_ERROR: %d", httpCode);
  }
  String response = http.getString();
  http.end();
  log_d("%s", response.c_str());
  return response;
}

String OpenAI::post(String endpoint, String jsonBody) {
  log_d("\"%s\": %s", endpoint.c_str(), jsonBody.c_str());
  HTTPClient http;
  http.setTimeout(60000);
  http.begin("https://api.openai.com/v1/" + endpoint);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer " + api_key);
  int httpCode = http.POST(jsonBody);
  if (httpCode != HTTP_CODE_OK) {
    log_e("HTTP_ERROR: %d", httpCode);
  }
  String response = http.getString();
  http.end();
  log_d("%s", response.c_str());
  return response;
}

String OpenAI::get(String endpoint) {
  log_d("\"%s\"", endpoint.c_str());
  HTTPClient http;
  http.begin("https://api.openai.com/v1/" + endpoint);
  http.addHeader("Authorization", "Bearer " + api_key);
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    log_e("HTTP_ERROR: %d", httpCode);
  }
  String response = http.getString();
  http.end();
  log_d("%s", response.c_str());
  return response;
}

String OpenAI::del(String endpoint) {
  log_d("\"%s\"", endpoint.c_str());
  HTTPClient http;
  http.begin("https://api.openai.com/v1/" + endpoint);
  http.addHeader("Authorization", "Bearer " + api_key);
  int httpCode = http.sendRequest("DELETE");
  if (httpCode != HTTP_CODE_OK) {
    log_e("HTTP_ERROR: %d", httpCode);
  }
  String response = http.getString();
  http.end();
  log_d("%s", response.c_str());
  return response;
}

OpenAI_Completion OpenAI::completion(){
  return OpenAI_Completion(*this);
}

OpenAI_ChatCompletion OpenAI::chat(){
  return OpenAI_ChatCompletion(*this);
}

OpenAI_Completion::OpenAI_Completion(OpenAI &openai)
  : oai(openai)
  , model(NULL)
  , max_tokens(0)
  , temperature(1)
  , top_p(1)
  , n(1)
  , echo(false)
  , stop(NULL)
  , presence_penalty(0)
  , frequency_penalty(0)
  , best_of(1)
  , user(NULL)
{}

OpenAI_Completion::~OpenAI_Completion(){
  if(model != NULL){
    free((void*)model);
  }
  if(stop != NULL){
    free((void*)stop);
  }
  if(user != NULL){
    free((void*)user);
  }
}

OpenAI_Completion & OpenAI_Completion::setModel(const char * m){
  if(model != NULL){
    free((void*)model);
  }
  model = strdup(m);
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setMaxTokens(unsigned int m){
  if(m > 0){
    max_tokens = m;
  }
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setTemperature(float t){
  if(t >= 0 && t <= 2.0){
    temperature = t;
  }
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setTopP(float t){
  if(t >= 0 && t <= 1.0){
    top_p = t;
  }
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setN(unsigned int _n){
  if(n > 0){
    n = _n;
  }
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setEcho(bool e){
  echo = e;
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setStop(const char * s){
  if(stop != NULL){
    free((void*)stop);
  }
  stop = strdup(s);
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setPresencePenalty(float p){
  if(p >= -2.0 && p <= 2.0){
    presence_penalty = p;
  }
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setFrequencyPenalty(float p){
  if(p >= -2.0 && p <= 2.0){
    frequency_penalty = p;
  }
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setBestOf(unsigned int b){
  if(b >= n){
    best_of = b;
  }
  return *this;
}

OpenAI_Completion & OpenAI_Completion::setUser(const char * u){
  if(user != NULL){
    free((void*)user);
  }
  user = strdup(u);
  return *this;
}

OpenAI_StringResponse OpenAI_Completion::prompt(String p){
  String endpoint = "completions";

  OpenAI_StringResponse result = OpenAI_StringResponse(NULL);
  cJSON * req = cJSON_CreateObject();
  if(req == NULL){
    log_e("cJSON_CreateObject failed!");
    return result;
  }
  reqAddString("model", (model == NULL)?"text-davinci-003":model);
  if(p.startsWith("[")){
    cJSON * in = cJSON_Parse(p.c_str());
    if(in == NULL || !cJSON_IsArray(in)){
      log_e("Input not JSON Array!");
      cJSON_Delete(req);
      return result;
    }
    reqAddItem("prompt", in);
  } else {
    reqAddString("prompt", p.c_str());
  }
  if(max_tokens){
    reqAddNumber("max_tokens", max_tokens);
  }
  if(temperature != 1){
    reqAddNumber("temperature", temperature);
  }
  if(top_p != 1){
    reqAddNumber("top_p", top_p);
  }
  if(n != 1){
    reqAddNumber("n", n);
  }
  if(echo){
    reqAddBool("echo", true);
  }
  if(stop != NULL){
    reqAddString("stop", stop);
  }
  if(presence_penalty != 0){
    reqAddNumber("presence_penalty", presence_penalty);
  }
  if(frequency_penalty != 0){
    reqAddNumber("frequency_penalty", frequency_penalty);
  }
  if(best_of != 1){
    reqAddNumber("best_of", best_of);
  }
  if(user != NULL){
    reqAddString("user", user);
  }
  String jsonBody = String(cJSON_Print(req));
  cJSON_Delete(req);
  String res = oai.post(endpoint, jsonBody);

  if(!res.length()){
    log_e("Empty result!");
    return result;
  }
  return OpenAI_StringResponse(res.c_str());
}

// chat/completions { //Given a chat conversation, the model will return a chat completion response.
//   "model": "gpt-3.5-turbo",//required
//   "messages": [//required array
//     {"role": "system", "content": "Description of the required assistant"},
//     {"role": "user", "content": "First question from the user"},
//     {"role": "assistant", "content": "Response from the assistant"},
//     {"role": "user", "content": "Next question from the user to be answered"}
//   ],
//   "temperature": 1,//float between 0 and 2
//   "top_p": 1,//float between 0 and 1. recommended to alter this or temperature but not both.
//   "stream": false,//boolean. Whether to stream back partial progress. keep false
//   "stop": null,//string or array. Up to 4 sequences where the API will stop generating further tokens. 
//   "max_tokens": 16,//integer. The maximum number of tokens to generate in the completion.
//   "presence_penalty": 0,//float between -2.0 and 2.0. Positive values penalize new tokens based on whether they appear in the text so far, increasing the model's likelihood to talk about new topics.
//   "frequency_penalty": 0,//float between -2.0 and 2.0. Positive values penalize new tokens based on their existing frequency in the text so far, decreasing the model's likelihood to repeat the same line verbatim.
//   "logit_bias": null,//map. Modify the likelihood of specified tokens appearing in the completion. 
//   "user": null//string. A unique identifier representing your end-user, which can help OpenAI to monitor and detect abuse.
// }

OpenAI_ChatCompletion::OpenAI_ChatCompletion(OpenAI &openai)
  : oai(openai)
  , model(NULL)
  , description(NULL)
  , max_tokens(0)
  , temperature(1)
  , top_p(1)
  , stop(NULL)
  , presence_penalty(0)
  , frequency_penalty(0)
  , user(NULL)
{
  messages = cJSON_CreateArray();
}

OpenAI_ChatCompletion::~OpenAI_ChatCompletion(){
  if(model != NULL){
    free((void*)model);
  }
  if(description != NULL){
    free((void*)description);
  }
  if(stop != NULL){
    free((void*)stop);
  }
  if(user != NULL){
    free((void*)user);
  }
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::setModel(const char * m){
  if(model != NULL){
    free((void*)model);
  }
  model = strdup(m);
  return *this;
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::setSystem(const char * s){
  if(description != NULL){
    free((void*)description);
  }
  description = strdup(s);
  return *this;
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::setMaxTokens(unsigned int m){
  if(m > 0){
    max_tokens = m;
  }
  return *this;
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::setTemperature(float t){
  if(t >= 0 && t <= 2.0){
    temperature = t;
  }
  return *this;
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::setTopP(float t){
  if(t >= 0 && t <= 1.0){
    top_p = t;
  }
  return *this;
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::setStop(const char * s){
  if(stop != NULL){
    free((void*)stop);
  }
  stop = strdup(s);
  return *this;
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::setPresencePenalty(float p){
  if(p >= -2.0 && p <= 2.0){
    presence_penalty = p;
  }
  return *this;
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::setFrequencyPenalty(float p){
  if(p >= -2.0 && p <= 2.0){
    frequency_penalty = p;
  }
  return *this;
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::setUser(const char * u){
  if(user != NULL){
    free((void*)user);
  }
  user = strdup(u);
  return *this;
}

OpenAI_ChatCompletion & OpenAI_ChatCompletion::clearConversation(){
  if(messages != NULL){
    cJSON_Delete(messages);
    messages = cJSON_CreateArray();
  }
  return *this;
}

static cJSON * createChatMessage(cJSON * messages, const char * role, const char * content){
  cJSON * message = cJSON_CreateObject();
  if(message == NULL){
    log_e("cJSON_CreateObject failed!");
    return NULL;
  }
  if(cJSON_AddStringToObject(message, "role", role) == NULL){
    cJSON_Delete(message);
    log_e("cJSON_AddStringToObject failed!");
    return NULL;
  }
  if(cJSON_AddStringToObject(message, "content", content) == NULL){
    cJSON_Delete(message);
    log_e("cJSON_AddStringToObject failed!");
    return NULL;
  }
  if(!cJSON_AddItemToArray(messages, message)){
    cJSON_Delete(message);
    log_e("cJSON_AddItemToArray failed!");
    return NULL;
  }
  return message;
}

OpenAI_StringResponse OpenAI_ChatCompletion::message(String p, bool save){
  String endpoint = "chat/completions";

  OpenAI_StringResponse result = OpenAI_StringResponse(NULL);
  cJSON * req = cJSON_CreateObject();
  if(req == NULL){
    log_e("cJSON_CreateObject failed!");
    return result;
  }
  reqAddString("model", (model == NULL)?"gpt-3.5-turbo":model);

  cJSON * _messages = cJSON_CreateArray();
  if(_messages == NULL){
    cJSON_Delete(req);
    log_e("cJSON_CreateArray failed!");
    return result;
  }
  if(description != NULL){
    if(createChatMessage(_messages, "system", description) == NULL){
      cJSON_Delete(req);
      cJSON_Delete(_messages);
      log_e("createChatMessage failed!");
      return result;
    }
  }
  if(messages != NULL && cJSON_IsArray(messages)){
    int mlen = cJSON_GetArraySize(messages);
    for(int i = 0; i < mlen; ++i){
      cJSON * item = cJSON_GetArrayItem(messages, i);
      if(item != NULL && cJSON_IsObject(item)){
        if(!cJSON_AddItemReferenceToArray(_messages, item)){
          cJSON_Delete(req);
          cJSON_Delete(_messages);
          log_e("cJSON_AddItemReferenceToArray failed!");
          return result;
        }
      }
    }
  }
  if(createChatMessage(_messages, "user", p.c_str()) == NULL){
    cJSON_Delete(req);
    cJSON_Delete(_messages);
    log_e("createChatMessage failed!");
    return result;
  }

  reqAddItem("messages", _messages);
  if(max_tokens){
    reqAddNumber("max_tokens", max_tokens);
  }
  if(temperature != 1){
    reqAddNumber("temperature", temperature);
  }
  if(top_p != 1){
    reqAddNumber("top_p", top_p);
  }
  if(stop != NULL){
    reqAddString("stop", stop);
  }
  if(presence_penalty != 0){
    reqAddNumber("presence_penalty", presence_penalty);
  }
  if(frequency_penalty != 0){
    reqAddNumber("frequency_penalty", frequency_penalty);
  }
  if(user != NULL){
    reqAddString("user", user);
  }
  String jsonBody = String(cJSON_Print(req));
  cJSON_Delete(req);

  String res = oai.post(endpoint, jsonBody);

  if(!res.length()){
    log_e("Empty result!");
    return result;
  }
  if(save){
    //add the responses to the messages here
    //double parsing is here as workaround
    OpenAI_StringResponse r = OpenAI_StringResponse(res.c_str());
    if(r.length()){
      if(createChatMessage(messages, "user", p.c_str()) == NULL){
        log_e("createChatMessage failed!");
      }
      if(createChatMessage(messages, "assistant", r.getAt(0)) == NULL){
        log_e("createChatMessage failed!");
      }
    }
  }
  return OpenAI_StringResponse(res.c_str());
}
