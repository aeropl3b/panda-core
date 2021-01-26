#pragma once


namespace xlib
{

class non_copyable
{
   non_copyable(const non_copyable&) = delete;
   non_copyable& operator=(const non_copyable&) = delete;
};

class non_moveable
{
   non_moveable(non_moveable&&) = delete;
   non_moveable& operator=(non_moveable&&) = delete;
};

class singleton: non_copyable, non_moveable
{};

}