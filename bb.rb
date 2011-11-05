#!/usr/bin/env ruby

$:.unshift File.dirname(__FILE__)
require 'extensions'

module BB
  class BB
    def initialize
      @tasks = []
      @best = nil
    end

    def read(reader=TaskReader.new)
      @tasks = reader.read
    end

    def print(printer=TaskPrinter.new)
      printer.print @tasks, @best
    end

    def compute
      @best = []
      @tasks.permutate do |a|
        @best = a if target_func(a) < target_func(@best) and not a.include? nil
        eliminate_func @best, a
      end
      @best
    end

    private
    def target_func(solution)
      time = 0
      result = solution.map do |t|
        if t == nil
          0
        else
          time += t.cost
          t.weight * [0, time - t.due].max
        end
      end.reduce(:+)
      result || Float::INFINITY
    end

    def eliminate_func(min, a)
      procedures = []
      procedures << lambda do
        target_func(a) > target_func(min.first(a.size))
      end

      procedures.map {|p| p.call }.include? true
    end
  end

  class Task
    attr_accessor :cost, :due, :weight, :id

    def initialize(options = {})
      options.each do |k,v|
        send "#{k}=", v
      end
    end
  end

  class TaskReader
    def read
      tasks = []
      index = 0
      STDIN.each_line do |l|
        fds = l.split
        index += 1
        tasks << Task.new(:cost => fds[0].to_i, :due => fds[1].to_i,
                          :weight => fds[2].to_i, :id => index)
      end
      tasks
    end
  end

  class TaskPrinter
    def print(tasks, best)
      puts " Nr | p_j | d_j | w_j"
      puts "----+-----+-----+-----"
      tasks.each do |t|
        puts "%2d. |%4d |%4d |%4d" % [t.id, t.cost, t.due, t.weight]
      end

      puts "\nBest solution is: #{best.map { |t| t.id }.join ', '}"
    end
  end
end

if $0 == __FILE__
  b = BB::BB.new
  b.read
  b.compute
  b.print
end

