//
// Created by stuart on 10/01/2026.
//

#ifndef GAME3_LLMTESTS_H
#define GAME3_LLMTESTS_H

#include <events/EventManager.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "EmbeddingLLM.h"
#include "SimpleLLM.h"
#include "StreamingLLM.h"
#include <file/SettingsManager.h>
#include <testlib/Messages.h>

#include "LLMPredctionCompleteEvent.h"
#include "LLMTokenPredictedReceived.h"

using namespace testing;

class LlmTests : public testing::Test
{
public:

    void SetUp() override
    {
        gamelib::SettingsManager::Get()->ReadSettingsFile("//home//stuart//repos//Game3//testdata//settings.xml");
    }

    void TearDown() override
    {
    }
};

class TestEventHandler : public gamelib::EventSubscriber
{
public:
    TestEventHandler(std::function<void(std::shared_ptr<gamelib::Event>)> eventReceivedCallback) : event_received_callback_(
        std::move(eventReceivedCallback)) {}

    void Init()
    {
        SubscribeToEvent(LLMPredictedTokenReceivedEventEventId);
        SubscribeToEvent(LLMPredictionCompleteEventEventId);
    }
    std::string GetSubscriberName() override {return "TestEventHandler";};

    std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(const std::shared_ptr<gamelib::Event> &evt,
        unsigned long deltaMs) override
    {
        event_received_callback_(evt);
        return {};
    }

    std::remove_reference<std::function<void(std::shared_ptr<gamelib::Event>)> &>::type event_received_callback_;

    [[nodiscard]] static int CountEventsReceived(std::vector<std::shared_ptr<gamelib::Event>> events, const gamelib::EventId &eventId)
    {
        const auto result = std::count_if(events.begin(), events.end(),
         [&](const std::shared_ptr<gamelib::Event> & event)
         {
             if( event->Id == eventId)
             {
                 return true;
             }
             return false;
         });
        return result;
    }

    template <typename T>
    std::vector<T> queue_to_vector(std::queue<T>& q)
    {
        std::queue<T> copy = q;  // copy first
        std::vector<T> result;
        result.reserve(copy.size());

        while (!copy.empty()) {
            result.push_back(copy.front());
            copy.pop();
        }

        return result;
    }
};

TEST_F(LlmTests, TestInferringModel)
{
    const auto inferringLLMModelPath = gamelib::SettingsManager::Get()->GetString("llm", "InferringLLMModelPath");

    SimpleLLM llm;

    StreamingLLM::RunWithoutStdErrOutput([&]()
    {
        // Supress output
        llm.Initialize(inferringLLMModelPath);
    });

    std::string response;
    StreamingLLM::RunWithoutStdErrOutput([&]()
    {
        // Supress output

        // Make 1 prediction
        response = llm.Infer("What is the largest city in France?", 10);
    });

    ASSERT_STREQ(response.c_str(), "\n\nAnswers: 1. Paris");

}

TEST_F(LlmTests, TestEmbeddingModel)
{
    // Load the path to the embedding model
    const auto embeddingModelPath = gamelib::SettingsManager::Get()->GetString("llm", "EmbeddingModelPath");

    std::vector<float> expectedEmbedding =
        {
            0.000420, -0.020891,0.004959,0.001576,0.006570,0.042280,0.024243,0.043384,0.016595,0.000514,-0.016195,
            -0.044983,-0.019867,0.042513,-0.064927,-0.034889,-0.014295,0.004836,-0.055993,0.012040,0.022841,
            -0.015734,0.006038,-0.021674,-0.004359,-0.009606,0.000448,0.027134,-0.068758,-0.085768,-0.005230,
            -0.044701,-0.004741,-0.029279,-0.009306,-0.011347,0.002612,-0.019967,0.059107,0.004180,0.052264,
            -0.037218,0.023883,-0.088922,0.066287,0.020753,-0.022964,0.087626,-0.011231,-0.056012,0.033779,
            0.019393,-0.018203,0.003743,-0.013329,0.042738,-0.022494,0.004412,0.025944,-0.016712,0.025884,
            0.027171,-0.226980,0.052193,-0.051393,0.043176,-0.001379,0.007552,-0.047162,-0.017588,-0.001381,
            0.056097,0.038241,0.012352,0.044315,-0.036642,-0.037131,-0.025671,-0.015862,-0.023628,0.078597,
            -0.031005,-0.007948,-0.028332,0.003899,-0.005780,-0.036726,0.032225,0.003354,-0.003011,0.007338,
            -0.067932,-0.013849,0.058347,-0.011050,-0.026545,0.054205,0.037039,0.100831,0.318058,-0.138552,
            0.041838,0.064727,0.009252,0.045951,-0.023312,-0.022410,0.021452,-0.036126,0.013581,0.003431,
            -0.052311,0.065976,-0.058572,-0.001718,-0.070737,0.087991,-0.009133,-0.018154,-0.019082,0.006013,
            0.000227,-0.012746,0.036102,0.047712,-0.000082,0.023053,0.042705,0.072326,-0.010251,0.009252,
            0.040291,-0.016499,0.051937,-0.047389,-0.023874,-0.035566,0.002488,0.003630,-0.003723,0.006758,
            -0.039623,0.000073,-0.040659,-0.023906,0.019272,0.042974,-0.036517,0.004893,0.006327,-0.008712,
            0.024727,-0.001334,0.030366,0.003591,0.091409,0.047917,0.017352,0.035507,-0.019433,0.022011,
            0.018107,-0.023835,0.096188,-0.003146,-0.105457,-0.056263,-0.013112,-0.054339,-0.008470,-0.005798,
            -0.002042,-0.006929,0.055813,0.086033,0.001379,-0.019244,0.041569,0.001295,-0.012206,0.056044,
            -0.011045,-0.041226,-0.009402,0.004169,-0.010141,-0.028378,-0.039707,0.018269,0.038293,0.030967,
            0.073613,0.021901,0.015930,-0.031910,0.016350,-0.024030,-0.071818,0.011647,-0.034740,-0.018421,
            -0.012017,0.007745,0.024674,0.007461,0.042543,0.029095,-0.032085,-0.059402,-0.016922,0.009974,
            0.084388,0.047512,-0.089936,0.022712,-0.010118,0.020217,-0.033282,0.025208,-0.016977,0.076148,
            -0.062892,-0.026881,-0.265245,-0.041806,0.009669,-0.039872,0.023561,0.042988,-0.037804,0.025809,
            0.047062,0.055773,0.015598,-0.024207,-0.029870,0.041500,-0.015468,-0.006610,0.010420,0.041415,
            -0.026453,-0.099184,0.001308,-0.003700,-0.079973,-0.067726,0.007260,0.020518,0.204640,0.003311,
            -0.034173,-0.003839,0.016317,-0.017706,0.019422,-0.113267,0.066272,0.005265,0.100471,0.014797,
            -0.054921,0.029828,0.059452,0.017527,-0.028231,0.004550,-0.049545,0.038721,0.054600,0.013641,
            -0.055501,-0.014128,-0.014438,-0.054267,0.029441,0.056077,0.007263,0.038945,-0.027607,0.024628,
            -0.012402,0.079969,-0.020196,-0.049066,0.024329,0.030882,0.024502,0.015911,-0.054234,-0.057031,
            -0.011178,0.041929,-0.014499,0.109188,-0.000563,-0.107503,0.045527,-0.004049,0.030352,0.056724,
            0.080474,0.011909,0.063596,-0.049981,-0.009538,-0.043423,-0.025800,-0.033538,0.087276,-0.044803,
            -0.014932,0.002999,-0.028882,-0.006477,-0.012765,-0.017222,-0.025001,-0.007218,-0.320194,0.017943,
            0.005811,0.043651,0.024978,0.037677,-0.026288,0.062397,-0.018341,-0.001322,0.073460,-0.061195,
            -0.013722,-0.005940,-0.014866,-0.031678,-0.064345,0.033034,0.018426,0.040503,0.012189,0.021971,
            0.114110,-0.078888,-0.003755,0.004979,-0.058871,0.040819,0.004526,-0.010039,-0.067800,-0.056829,
            0.012992,-0.060127,-0.041921,0.070261,-0.037899,0.052562,-0.016257,-0.068266,-0.052888,0.058793,
            0.086513,-0.047714,0.047860,-0.006568,0.016841,0.009080,0.034291,0.000692,0.033271,-0.021265,
            -0.008144,0.030122,-0.010515,-0.042425,-0.011365,-0.050647,0.084317,0.063261,-0.023363,-0.002910,
            -0.057310,-0.001854,0.027290
    };


    EmbeddingLLM embeddingModel;
    StreamingLLM::RunWithoutStdErrOutput([&]()
    {
        // Supress output
        embeddingModel.Initialize(embeddingModelPath);
    });

    const auto actual = embeddingModel.GetEmbedding();

    for (size_t i = 0; i < actual.size(); ++i)
    {
        constexpr double eps = 1e-6;
        EXPECT_NEAR(actual[i], expectedEmbedding[i], eps);
    }
}

TEST_F(LlmTests, TestStreamingLLM)
{
    StreamingLLM streamingLLm;

    // Initialize with a prompt
    streamingLLm.Initialize("What is the largest city in England?", 10);

    int tokensReceived = 0;
    // Register a callback in the Test handler that will receive the events submitted by the streaming llm
    TestEventHandler testEventHandler([&](const std::shared_ptr<gamelib::Event> &evt)
    {
        tokensReceived++;

        // This is called when a streaming llm event is received by the test handler
        if (evt->Id == LLMPredictedTokenReceivedEventEventId)
        {
            const auto receivedEvent = std::static_pointer_cast<LLMPredictedTokenReceivedEvent>(evt);
            std::cout << tokensReceived << ": new token received: " << receivedEvent->token << std::endl;
        }
        if (evt->Id == LLMPredictionCompleteEventEventId)
        {
            std::cout << "** prediction complete event received. **\n";
        }
    });

    testEventHandler.Init();

    StreamingLLM::RunWithoutStdErrOutput([&]()
    {
        streamingLLm.Update(0.0f);
    });

    auto eventManager = gamelib::EventManager::Get();

    auto events = eventManager->GetEvents();

    eventManager->ProcessAllEvents();
    const auto countPredictedTokenReceivedEvents = testEventHandler.CountEventsReceived(testEventHandler.queue_to_vector(events),
                                         LLMPredictedTokenReceivedEventEventId);
    const auto countCompletionEvents = testEventHandler.CountEventsReceived(testEventHandler.queue_to_vector(events),
                                        LLMPredictionCompleteEventEventId);
    ASSERT_EQ(11, countPredictedTokenReceivedEvents);
    ASSERT_EQ(1, countCompletionEvents);
}






#endif //GAME3_LLMTESTS_H