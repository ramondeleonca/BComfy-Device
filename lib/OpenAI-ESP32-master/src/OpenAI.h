#pragma once
#include "Arduino.h"
#include "cJSON.h"

// ! HEAVILY MODIFIED FOR BCOMFY, REMOVED UNUSED FUNCTIONS AND CLASSES

class OpenAI_Completion;
class OpenAI_ChatCompletion;
class OpenAI_StringResponse {
  private:
    unsigned int usage;
    unsigned int len;
    char ** data;
    char * error_str;

  public:
    OpenAI_StringResponse(const char * payload);
    ~OpenAI_StringResponse();

    unsigned int tokens(){
      return usage;
    }
    unsigned int length(){
      return len;
    }
    const char * getAt(unsigned int index){
      if(index < len){
        return data[index];
      }
      return "";
    }
    const char * error(){
      return error_str;
    }
};

class OpenAI {
  private:
    String api_key;

  protected:

  public:
    OpenAI(const char *openai_api_key);
    ~OpenAI();

    OpenAI_Completion completion();
    OpenAI_ChatCompletion chat();

    String get(String endpoint);
    String del(String endpoint);
    String post(String endpoint, String jsonBody);
    String upload(String endpoint, String boundary, uint8_t * data, size_t len);
};

class OpenAI_Completion {
  private:
    OpenAI & oai;
    const char * model;
    unsigned int max_tokens;
    float temperature;
    float top_p;
    unsigned int n;
    bool echo;
    const char * stop;
    float presence_penalty;
    float frequency_penalty;
    unsigned int best_of;
    const char * user;

  protected:

  public:
    OpenAI_Completion(OpenAI &openai);
    ~OpenAI_Completion();

    OpenAI_Completion & setModel(const char * m);
    OpenAI_Completion & setMaxTokens(unsigned int m); //The maximum number of tokens to generate in the completion.
    OpenAI_Completion & setTemperature(float t);      //float between 0 and 2. Higher value gives more random results.
    OpenAI_Completion & setTopP(float t);             //float between 0 and 1. recommended to alter this or temperature but not both.
    OpenAI_Completion & setN(unsigned int n);         //How many completions to generate for each prompt.
    OpenAI_Completion & setEcho(bool e);              //Echo back the prompt in addition to the completion
    OpenAI_Completion & setStop(const char * s);      //Up to 4 sequences where the API will stop generating further tokens.
    OpenAI_Completion & setPresencePenalty(float p);  //float between -2.0 and 2.0. Positive values increase the model's likelihood to talk about new topics.
    OpenAI_Completion & setFrequencyPenalty(float p); //float between -2.0 and 2.0. Positive values decrease the model's likelihood to repeat the same line verbatim.
    OpenAI_Completion & setBestOf(unsigned int b);    //Generates best_of completions server-side and returns the "best". "best_of" must be greater than "n"
    OpenAI_Completion & setUser(const char * u);      //A unique identifier representing your end-user, which can help OpenAI to monitor and detect abuse.

    OpenAI_StringResponse prompt(String p);           //Send the prompt for completion
};

class OpenAI_ChatCompletion {
  private:
    OpenAI & oai;
    cJSON * messages;
    const char * model;
    const char * description;
    unsigned int max_tokens;
    float temperature;
    float top_p;
    const char * stop;
    float presence_penalty;
    float frequency_penalty;
    const char * user;

  protected:

  public:
    OpenAI_ChatCompletion(OpenAI &openai);
    ~OpenAI_ChatCompletion();

    OpenAI_ChatCompletion & setModel(const char * m);
    OpenAI_ChatCompletion & setSystem(const char * s);    //Description of the required assistant
    OpenAI_ChatCompletion & setMaxTokens(unsigned int m); //The maximum number of tokens to generate in the completion.
    OpenAI_ChatCompletion & setTemperature(float t);      //float between 0 and 2. Higher value gives more random results.
    OpenAI_ChatCompletion & setTopP(float t);             //float between 0 and 1. recommended to alter this or temperature but not both.
    OpenAI_ChatCompletion & setStop(const char * s);      //Up to 4 sequences where the API will stop generating further tokens.
    OpenAI_ChatCompletion & setPresencePenalty(float p);  //float between -2.0 and 2.0. Positive values increase the model's likelihood to talk about new topics.
    OpenAI_ChatCompletion & setFrequencyPenalty(float p); //float between -2.0 and 2.0. Positive values decrease the model's likelihood to repeat the same line verbatim.
    OpenAI_ChatCompletion & setUser(const char * u);      //A unique identifier representing your end-user, which can help OpenAI to monitor and detect abuse.
    OpenAI_ChatCompletion & clearConversation();          //clears the accumulated conversation

    OpenAI_StringResponse message(String m, bool save=true);//Send the message for completion. Save it with the first response if selected
};