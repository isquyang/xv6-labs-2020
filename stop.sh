ps -elf | grep -i qemu | grep -v 'grep' | awk '{print $2}' | xargs kill -9
