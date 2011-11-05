class NilClass
  def try(method, *args, &block)
    nil
  end
end

class BasicObject
  def try(method, *args, &block)
    send method, *args, &block if respond_to? method
  end
end

class Array
  def permutate(&block)
    array = Array.new size, nil
    permutate_re array, 0, block
  end

  private
  def permutate_re(array, level = 0, block) # :nodoc:
    unless level == array.size
      array = array.dup
      each do |task|
        next if array.include? task
        array[level] = task
        unless block[array]
          permutate_re array, level + 1, block
        end
      end
    end
  end
end

