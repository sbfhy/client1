#pragma once

class noncopyable
{
public:
  noncopyable(const noncopyable&) = delete;
  void operator = (const noncopyable&) = delete;

protected:
  noncopyable() = default;
  virtual ~noncopyable() = default;
};
