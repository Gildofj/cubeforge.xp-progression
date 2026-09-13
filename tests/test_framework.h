#ifndef XP_PROGRESSION_TEST_FRAMEWORK_H
#define XP_PROGRESSION_TEST_FRAMEWORK_H

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <functional>
#include <sstream>
#include <iomanip>
#include <windows.h>

namespace xp_progression::testing {

    struct TestFailure {
        std::string expression;
        std::string file;
        int line;
        std::string message;
    };

    struct TestCaseInfo {
        std::string suiteName;
        std::string testName;
        std::function<void()> testFunc;
    };

    class TestRunner {
    private:
        std::vector<TestCaseInfo> m_tests;
        std::vector<TestFailure> m_currentFailures;
        int m_passedTests = 0;
        int m_failedTests = 0;

    public:
        static TestRunner& Instance() {
            static TestRunner runner;
            return runner;
        }

        void RegisterTest(const std::string& suiteName, const std::string& testName, std::function<void()> func) {
            m_tests.push_back({ suiteName, testName, std::move(func) });
        }

        void AddFailure(const std::string& expr, const std::string& file, int line, const std::string& msg = "") {
            m_currentFailures.push_back({ expr, file, line, msg });
        }

        int RunAllTests() {
            m_passedTests = 0;
            m_failedTests = 0;

            std::cout << "\n=======================================================\n";
            std::cout << "       XP Progression Test Suite Execution Runner     \n";
            std::cout << "=======================================================\n" << std::flush;

            auto globalStart = std::chrono::high_resolution_clock::now();

            std::string currentSuite = "";
            for (const auto& test : m_tests) {
                if (test.suiteName != currentSuite) {
                    currentSuite = test.suiteName;
                    std::cout << "\n\033[1;36m[SUITE]\033[0m " << currentSuite << "\n";
                    std::cout << "-------------------------------------------------------\n" << std::flush;
                }

                std::cout << "  \033[1;33m[RUN ]\033[0m " << test.testName << "..." << std::flush;

                m_currentFailures.clear();
                auto start = std::chrono::high_resolution_clock::now();

                try {
                    test.testFunc();
                }
                catch (const std::exception& e) {
                    AddFailure("std::exception thrown", __FILE__, __LINE__, e.what());
                }
                catch (...) {
                    AddFailure("Unknown exception thrown", __FILE__, __LINE__);
                }

                auto end = std::chrono::high_resolution_clock::now();
                double elapsedMs = std::chrono::duration<double, std::milli>(end - start).count();

                if (m_currentFailures.empty()) {
                    m_passedTests++;
                    std::cout << "\r  \033[1;32m[PASS]\033[0m " << test.testName 
                              << " (" << std::fixed << std::setprecision(2) << elapsedMs << " ms)\n" << std::flush;
                }
                else {
                    m_failedTests++;
                    std::cout << "\r  \033[1;31m[FAIL]\033[0m " << test.testName 
                              << " (" << std::fixed << std::setprecision(2) << elapsedMs << " ms)\n" << std::flush;
                    for (const auto& failure : m_currentFailures) {
                        std::cout << "         \033[31m--> Assertion Failed:\033[0m " << failure.expression << "\n"
                                  << "             Location: " << failure.file << ":" << failure.line << "\n";
                        if (!failure.message.empty()) {
                            std::cout << "             Detail: " << failure.message << "\n";
                        }
                    }
                    std::cout << std::flush;
                }
            }

            auto globalEnd = std::chrono::high_resolution_clock::now();
            double totalMs = std::chrono::duration<double, std::milli>(globalEnd - globalStart).count();

            std::cout << "\n=======================================================\n";
            std::cout << "                 Test Execution Summary                \n";
            std::cout << "=======================================================\n";
            std::cout << "Total Test Cases: " << (m_passedTests + m_failedTests) << "\n";
            std::cout << "\033[1;32mPassed:\033[0m " << m_passedTests << "\n";
            if (m_failedTests > 0) {
                std::cout << "\033[1;31mFailed:\033[0m " << m_failedTests << "\n";
            } else {
                std::cout << "Failed: 0\n";
            }
            std::cout << "Total Elapsed Time: " << std::fixed << std::setprecision(2) << totalMs << " ms\n";
            std::cout << "=======================================================\n\n" << std::flush;

            return m_failedTests == 0 ? 0 : 1;
        }
    };

} // namespace xp_progression::testing

#define TEST_FUNC(suite_name, test_name) void test_##suite_name##_##test_name()

#define REGISTER_TEST(suite_name, test_name) \
    ::xp_progression::testing::TestRunner::Instance().RegisterTest(#suite_name, #test_name, test_##suite_name##_##test_name)

#define ASSERT_TRUE(condition) \
    do { \
        if (!(condition)) { \
            ::xp_progression::testing::TestRunner::Instance().AddFailure(#condition, __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        if (condition) { \
            ::xp_progression::testing::TestRunner::Instance().AddFailure("!(" #condition ")", __FILE__, __LINE__); \
        } \
    } while(0)

#define ASSERT_EQ(actual, expected) \
    do { \
        auto _act = (actual); \
        auto _exp = (expected); \
        if (!(_act == _exp)) { \
            std::stringstream _ss; \
            _ss << "Expected: [" << _exp << "] but got: [" << _act << "]"; \
            ::xp_progression::testing::TestRunner::Instance().AddFailure(#actual " == " #expected, __FILE__, __LINE__, _ss.str()); \
        } \
    } while(0)

#define ASSERT_NE(actual, expected) \
    do { \
        auto _act = (actual); \
        auto _exp = (expected); \
        if (_act == _exp) { \
            std::stringstream _ss; \
            _ss << "Expected values to differ, but both were: [" << _act << "]"; \
            ::xp_progression::testing::TestRunner::Instance().AddFailure(#actual " != " #expected, __FILE__, __LINE__, _ss.str()); \
        } \
    } while(0)

#define ASSERT_LT(val1, val2) \
    do { \
        if (!((val1) < (val2))) { \
            std::stringstream _ss; \
            _ss << "[" << (val1) << "] is not < [" << (val2) << "]"; \
            ::xp_progression::testing::TestRunner::Instance().AddFailure(#val1 " < " #val2, __FILE__, __LINE__, _ss.str()); \
        } \
    } while(0)

#define ASSERT_LE(val1, val2) \
    do { \
        if (!((val1) <= (val2))) { \
            std::stringstream _ss; \
            _ss << "[" << (val1) << "] is not <= [" << (val2) << "]"; \
            ::xp_progression::testing::TestRunner::Instance().AddFailure(#val1 " <= " #val2, __FILE__, __LINE__, _ss.str()); \
        } \
    } while(0)

#define ASSERT_GT(val1, val2) \
    do { \
        if (!((val1) > (val2))) { \
            std::stringstream _ss; \
            _ss << "[" << (val1) << "] is not > [" << (val2) << "]"; \
            ::xp_progression::testing::TestRunner::Instance().AddFailure(#val1 " > " #val2, __FILE__, __LINE__, _ss.str()); \
        } \
    } while(0)

#define ASSERT_GE(val1, val2) \
    do { \
        if (!((val1) >= (val2))) { \
            std::stringstream _ss; \
            _ss << "[" << (val1) << "] is not >= [" << (val2) << "]"; \
            ::xp_progression::testing::TestRunner::Instance().AddFailure(#val1 " >= " #val2, __FILE__, __LINE__, _ss.str()); \
        } \
    } while(0)

#define ASSERT_NEAR(actual, expected, epsilon) \
    do { \
        auto _act = (actual); \
        auto _exp = (expected); \
        auto _eps = (epsilon); \
        if (std::abs(_act - _exp) > _eps) { \
            std::stringstream _ss; \
            _ss << "Expected [" << _exp << "] +/- " << _eps << " but got [" << _act << "] (diff: " << std::abs(_act - _exp) << ")"; \
            ::xp_progression::testing::TestRunner::Instance().AddFailure("|" #actual " - " #expected "| <= " #epsilon, __FILE__, __LINE__, _ss.str()); \
        } \
    } while(0)

#define ASSERT_STREQ(actual, expected) \
    do { \
        std::string _act(actual); \
        std::string _exp(expected); \
        if (_act != _exp) { \
            std::stringstream _ss; \
            _ss << "Expected: \"" << _exp << "\" but got: \"" << _act << "\""; \
            ::xp_progression::testing::TestRunner::Instance().AddFailure(#actual " == " #expected, __FILE__, __LINE__, _ss.str()); \
        } \
    } while(0)

#endif // XP_PROGRESSION_TEST_FRAMEWORK_H
