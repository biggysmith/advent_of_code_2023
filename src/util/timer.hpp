#include <chrono>

class timer
{
public:
    void start() {
        start_time_ = std::chrono::system_clock::now();
        running_ = true;
    }
    
    void stop() {
        end_time_ = std::chrono::system_clock::now();
        running_ = false;
    }
    
    double microseconds() {
        std::chrono::time_point<std::chrono::system_clock> end_time;
        
        if(running_) {
            end_time = std::chrono::system_clock::now();
        } else {
            end_time = end_time_;
        }
        
        return (double)std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_).count();
    }
    
    double milliseconds() {
        std::chrono::time_point<std::chrono::system_clock> end_time;
        
        if(running_) {
            end_time = std::chrono::system_clock::now();
        } else {
            end_time = end_time_;
        }
        
        return (double)std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_).count();
    }
    
    double seconds() {
        return milliseconds() / 1000.0;
    } 

private:
    std::chrono::time_point<std::chrono::system_clock> start_time_;
    std::chrono::time_point<std::chrono::system_clock> end_time_;
    bool running_ = false;
};

enum timer_mode_enum { e_microseconds=0, e_milliseconds, e_seconds };

class scoped_timer
{
public:
    scoped_timer(timer_mode_enum mode=e_milliseconds) : mode_(mode) {
        timer_.start();
    }  

    ~scoped_timer() {
        timer_.stop();
        if(mode_ == e_microseconds){
            std::cout << timer_.microseconds() << "us" << std::endl;
        }else if(mode_ == e_milliseconds){
            std::cout << timer_.milliseconds() << "ms" << std::endl;
        }else if(mode_ == e_seconds){
            std::cout << timer_.seconds() << "s" << std::endl;
        }
    }

private:
    timer timer_;
    timer_mode_enum mode_;
};
