.DATA

.WEAK	"%eax"
.WEAK	"%ebx"
.WEAK	"%ecx"
.WEAK	"%r0"
.WEAK	"%r1"
.WEAK	"%r2"
.WEAK	"%r3"
hFAT_FILE_SYSTEM	.DW	1	?
.GLOBAL	  DO_NOT_EXPORT "hFAT_FILE_SYSTEM"
tcbREPROGRAMMING	.DW	1	?
.GLOBAL	  DO_NOT_EXPORT "tcbREPROGRAMMING"
hBOMS	.DW	1	?
.GLOBAL	  DO_NOT_EXPORT "hBOMS"
hUART	.DW	1	?
.GLOBAL	  DO_NOT_EXPORT "hUART"
tcbBUS	.DW	1	?
.GLOBAL	  DO_NOT_EXPORT "tcbBUS"
hUSBHOST_1	.DW	1	?
.GLOBAL	  DO_NOT_EXPORT "hUSBHOST_1"
hSPI_SLAVE_0	.DW	1	?
.GLOBAL	  DO_NOT_EXPORT "hSPI_SLAVE_0"
hSPI_SLAVE_1	.DW	1	?
.GLOBAL	  DO_NOT_EXPORT "hSPI_SLAVE_1"
hGPIO_PORT_A	.DW	1	?
.GLOBAL	  DO_NOT_EXPORT "hGPIO_PORT_A"
Str@0	.ASCIIZ	"TT&C"
Str@1	.ASCIIZ	"Reprogramming"




.TEXT

Array@2	.DB	12	53, 46, -8, 83, 2, 21, 0, 0, 0, 1, -69, 21
Array@3	.DB	12	53, 46, -8, 83, 2, 32, 0, 0, 0, 1, -28, -125
Array@4	.DB	12	53, 46, -8, 83, 2, 37, 0, 0, 0, 1, 92, -115
Array@5	.DB	12	53, 46, -8, 83, 2, 48, 0, 0, 0, 1, -69, -58

.WEAK	"vos_dma_get_fifo_flow_control"

.WEAK	"vos_start_scheduler"

.WEAK	"vos_gpio_write_port"

.WEAK	"vos_signal_semaphore_from_isr"

.WEAK	"vos_malloc"

.WEAK	"vos_create_thread_ex"

.WEAK	"vos_memcpy"

.WEAK	"vos_memset"

.WEAK	"vos_get_kernel_clock"

.WEAK	"vos_gpio_disable_int"

.WEAK	"vos_get_package_type"

.WEAK	"vos_dma_get_fifo_data_register"

.WEAK	"fat_dirTableFindFirst"

.WEAK	"vos_signal_semaphore"

.WEAK	"fat_fileMod"

.WEAK	"vos_gpio_wait_on_int"

.WEAK	"stdinAttach"

.WEAK	"stdioAttach"

.WEAK	"vos_dma_get_fifo_data"

.WEAK	"fatdrv_init"

.WEAK	"vos_iocell_get_config"

.WEAK	"vos_iomux_define_bidi"

.WEAK	"vos_gpio_set_all_mode"

.WEAK	"vos_iocell_set_config"

.WEAK	"fat_fileRead"

.WEAK	"vos_gpio_set_pin_mode"

.WEAK	"iomux_setup"

.WEAK	"fat_fileSeek"

.WEAK	"fat_dirEntryIsReadOnly"

.WEAK	"vos_get_chip_revision"

.WEAK	"fat_fileTell"

.WEAK	"vos_wait_semaphore_ex"

.WEAK	"fat_fileOpen"

.WEAK	"fat_fileCopy"

.WEAK	"vos_enable_interrupts"

.WEAK	"fat_capacity"

.WEAK	"stderrAttach"

.WEAK	"vos_dev_read"

.WEAK	"stdoutAttach"

.WEAK	"vos_dev_open"

.WEAK	"vos_halt_cpu"

.WEAK	"vos_dev_init"

.WEAK	"vos_dma_get_fifo_count"

.WEAK	"fat_getFSType"

.WEAK	"usbhost_init"

.WEAK	"vos_reset_kernel_clock"

.WEAK	"fat_freeSpace"

.WEAK	"fat_fileClose"

.WEAK	"abs"

.WEAK	"fat_dirIsRoot"

.WEAK	"vos_gpio_set_port_mode"

.WEAK	"fat_fileFlush"

.WEAK	"vos_iomux_define_input"

.WEAK	"fat_fileWrite"

.WEAK	"vos_disable_interrupts"

.WEAK	"fat_dirEntryIsDirectory"

.WEAK	"vos_get_idle_thread_tcb"

.WEAK	"vos_dma_reset"

.WEAK	"vos_dev_close"

.WEAK	"vos_wdt_clear"

.WEAK	"vos_heap_size"

.WEAK	"vos_dev_ioctl"

.WEAK	"spislave_init"

.WEAK	"vos_dev_write"

.WEAK	"fat_fileDelete"

.WEAK	"fat_fileRename"

.WEAK	"vos_get_clock_frequency"

.WEAK	"fat_fileSetPos"

.WEAK	"vos_set_clock_frequency"

.WEAK	"feof"

.WEAK	"free"

.WEAK	"fat_fileRewind"

.WEAK	"itoa"

.WEAK	"atoi"

.WEAK	"vos_dma_enable"

.WEAK	"ltoa"

.WEAK	"atol"

.WEAK	"vos_reset_vnc2"

.WEAK	"vos_heap_space"

.WEAK	"vos_iomux_define_output"

.WEAK	"vos_wdt_enable"

.WEAK	"fat_getVolumeID"

.WEAK	"vos_dma_wait_on_complete"

.WEAK	"vos_lock_mutex"

.WEAK	"vos_power_down"

.WEAK	"vos_init_mutex"

.WEAK	"fat_dirEntryIsVolumeLabel"

.WEAK	"fread"

.WEAK	"vos_gpio_wait_on_any_int"

.WEAK	"fgetc"

.WEAK	"chdir"

.WEAK	"fseek"

.WEAK	"vos_get_priority_ceiling"

.WEAK	"ftell"

.WEAK	"fopen"

.WEAK	"fgets"

.WEAK	"vos_dma_disable"

.WEAK	"vos_set_priority_ceiling"

.WEAK	"fputc"

.WEAK	"vos_dma_release"

.WEAK	"vos_iomux_disable_output"

.WEAK	"fputs"

.WEAK	"vos_dma_acquire"

.WEAK	"fat_dirChangeDir"

.WEAK	"vos_delay_msecs"

.WEAK	"vos_stack_usage"

.WEAK	"fat_dirTableFind"

.WEAK	"fat_getDevHandle"

.WEAK	"vos_get_profile"

.WEAK	"fat_dirCreateDir"

.WEAK	"vos_gpio_wait_on_all_ints"

.WEAK	"fat_dirEntryName"

.WEAK	"calloc"

.WEAK	"malloc"

.WEAK	"rename"

.WEAK	"fat_dirEntryTime"

.WEAK	"fclose"

.WEAK	"fat_fileTruncate"

.WEAK	"fat_dirEntrySize"

.WEAK	"fflush"

.WEAK	"rewind"

.WEAK	"memset"

.WEAK	"memcpy"

.WEAK	"vos_delay_cancel"

.WEAK	"remove"

.WEAK	"strcat"

.WEAK	"fwrite"

.WEAK	"printf"

.WEAK	"strlen"

.WEAK	"strcmp"

.WEAK	"strcpy"

.WEAK	"strtol"

.WEAK	"vos_dma_retained_configure"

.WEAK	"fat_dirDirIsEmpty"

.WEAK	"vos_unlock_mutex"

.WEAK	"getchar"

.WEAK	"putchar"

.WEAK	"fgetpos"

.WEAK	"fprintf"

.WEAK	"vos_gpio_read_all"

.WEAK	"vos_create_thread"

.WEAK	"fsetpos"

.WEAK	"sprintf"

.WEAK	"strncmp"

.WEAK	"vos_gpio_read_pin"

.WEAK	"vos_dma_configure"

.WEAK	"strncpy"

.WEAK	"vos_init_cond_var"

.WEAK	"vos_wait_cond_var"

.WEAK	"fat_dirEntryIsFile"

.WEAK	"fsAttach"

.WEAK	"initHeap"

.WEAK	"fat_getVolumeLabel"

.WEAK	"vos_stop_profiler"

.WEAK	"fat_time"

.WEAK	"fat_open"

.WEAK	"fat_init"

.WEAK	"vos_trylock_mutex"

.WEAK	"fat_bytesPerSector"

.WEAK	"vos_free"

.WEAK	"vos_init"

.WEAK	"vos_gpio_read_port"

.WEAK	"vos_gpio_write_all"

.WEAK	"vos_set_idle_thread_tcb_size"

.WEAK	"vos_init_semaphore"

.WEAK	"vos_wait_semaphore"

.WEAK	"vos_gpio_write_pin"

.WEAK	"fat_dirEntryIsValid"

.WEAK	"vos_start_profiler"

.WEAK	"fat_close"

.WEAK	"gpio_init"

.WEAK	"boms_init"

.WEAK	"fat_bytesPerCluster"

.WEAK	"uart_init"

.WEAK	"vos_gpio_enable_int"

.WEAK	"vos_signal_cond_var"

.WEAK	"fat_dirTableFindNext"

main:	
.GLOBAL	 DO_NOT_EXPORT  "main"

.FUNCTION	"main"	
PUSH32	%r0
PUSH32	%r1
SP_DEC	$21
PUSH8	$7
PUSH16	$1
PUSH8	$50
CALL	vos_init
SP_INC	$4
PUSH8	$0
CALL	vos_set_clock_frequency
SP_INC	$1
PUSH16	$512
CALL	vos_set_idle_thread_tcb_size
SP_INC	$2
CALL	iomux_setup
SP_STORE	%r0
CPY16	%r1	%r0
LD8	(%r1)	$192
PUSH16	%r0
PUSH8	$1
SP_DEC	$1
CALL	uart_init
POP8	%eax
SP_WR8	%eax	$4
SP_INC	$3
SP_STORE	%r0
INC16	%r0	$2
LD16	%r1	$1
ADD16	%r1	%r0
LD8	(%r1)	$192
CPY16	%r1	%r0
LD8	(%r1)	$0
PUSH16	%r0
PUSH8	$2
SP_DEC	$1
CALL	spislave_init
POP8	%eax
SP_WR8	%eax	$7
SP_INC	$3
SP_STORE	%r0
INC16	%r0	$5
LD16	%r1	$1
ADD16	%r1	%r0
LD8	(%r1)	$192
CPY16	%r1	%r0
LD8	(%r1)	$1
PUSH16	%r0
PUSH8	$3
SP_DEC	$1
CALL	spislave_init
POP8	%eax
SP_WR8	%eax	$10
SP_INC	$3
SP_STORE	%r0
INC16	%r0	$8
CPY16	%r1	%r0
LD8	(%r1)	$0
PUSH16	%r0
PUSH8	$4
SP_DEC	$1
CALL	gpio_init
POP8	%eax
SP_WR8	%eax	$12
SP_INC	$3
PUSH8	$5
SP_DEC	$1
CALL	fatdrv_init
POP8	%eax
SP_WR8	%eax	$11
SP_INC	$1
PUSH8	$6
SP_DEC	$1
CALL	boms_init
POP8	%eax
SP_WR8	%eax	$12
SP_INC	$1
SP_STORE	%r0
INC16	%r0	$12
CPY16	%r1	%r0
LD8	(%r1)	$8
LD16	%r1	$1
ADD16	%r1	%r0
LD8	(%r1)	$16
LD16	%r1	$2
ADD16	%r1	%r0
LD8	(%r1)	$2
LD16	%r1	$3
ADD16	%r1	%r0
LD8	(%r1)	$2
PUSH16	%r0
PUSH8	$-1
PUSH8	$0
SP_DEC	$1
CALL	usbhost_init
POP8	%eax
SP_WR8	%eax	$20
SP_INC	$4
LD32	%r0	$bus
LD32	%r1	$Str@0
PUSH16	$0
PUSH16	%r1
PUSH32	%r0
PUSH16	$2028
PUSH8	$20
SP_DEC	$2
CALL	vos_create_thread_ex
POP16	%eax
SP_WR16	%eax	$28
SP_INC	$11
SP_RD16	tcbBUS	$17
LD32	%r0	$reprogramming
LD32	%r1	$Str@1
PUSH16	$0
PUSH16	%r1
PUSH32	%r0
PUSH16	$4096
PUSH8	$24
SP_DEC	$2
CALL	vos_create_thread_ex
POP16	%eax
SP_WR16	%eax	$30
SP_INC	$11
SP_RD16	tcbREPROGRAMMING	$19
CALL	vos_start_scheduler
@fl1main_loop:	
JUMP	@fl1main_loop
SP_INC	$21
POP32	%r1
POP32	%r0
RTS	
.FUNC_END	"main"

uartsetup:	
.GLOBAL	 DO_NOT_EXPORT  "uartsetup"

.FUNCTION	"uartsetup"	
PUSH32	%r0
PUSH32	%r1
PUSH32	%r2
PUSH32	%r3
SP_DEC	$15
SP_RD16	%r0	$34
SP_STORE	%r1
INC16	%r1	$0
CPY16	%r2	%r1
LD8	(%r2)	$4
LD16	%r2	$1
ADD16	%r2	%r1
CPY16	%r3	%r2
LD8	(%r3)	$0
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$13
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$34
CPY16	%r3	%r2
LD32	(%r3)	$921600
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$14
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$35
CPY16	%r3	%r2
LD8	(%r3)	$0
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$15
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$36
CPY16	%r3	%r2
LD8	(%r3)	$1
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$16
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$37
CPY16	%r3	%r2
LD8	(%r3)	$0
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$17
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$38
CPY16	%r2	%r2
LD8	(%r2)	$0
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$18
SP_INC	$4
SP_INC	$15
POP32	%r3
POP32	%r2
POP32	%r1
POP32	%r0
RTS	
.FUNC_END	"uartsetup"

check_sync:	
.GLOBAL	 DO_NOT_EXPORT  "check_sync"

.FUNCTION	"check_sync"	
PUSH32	%r0
PUSH32	%r1
SP_RD16	%r0	$12
CPY8	%eax	(%r0)
AND32	%eax	$255
CPY32	%r1	%eax
CMP32	%r1	$53
JNZ	@IC2
@IC6:	
LD16	%r1	$1
ADD16	%r1	%r0
CPY8	%r1	(%r1)
AND32	%r1	$255
CMP32	%r1	$46
JNZ	@IC2
@IC5:	
LD16	%r1	$2
ADD16	%r1	%r0
CPY8	%r1	(%r1)
AND32	%r1	$255
CMP32	%r1	$248
JNZ	@IC2
@IC4:	
LD16	%r1	$3
ADD16	%r1	%r0
CPY8	%r1	(%r1)
AND32	%r1	$255
CMP32	%r1	$83
JNZ	@IC2
@IC3:	
LD8	%eax	$1
SP_WR8	%eax	$11
POP32	%r1
POP32	%r0
RTS	
JUMP	@IC1
@IC2:	
LD8	%eax	$0
SP_WR8	%eax	$11
POP32	%r1
POP32	%r0
RTS	
@IC1:	
POP32	%r1
POP32	%r0
RTS	
.FUNC_END	"check_sync"

reprogramming:	
.GLOBAL	 DO_NOT_EXPORT  "reprogramming"

.FUNCTION	"reprogramming"	
PUSH32	%r0
PUSH32	%r1
PUSH32	%r2
PUSH32	%r3
SP_DEC	$255
SP_DEC	$255
SP_DEC	$78
PUSH8	$1
SP_DEC	$2
CALL	vos_dev_open
POP16	%eax
SP_WR16	%eax	$1
SP_INC	$1
SP_RD16	%r0	$0
PUSH8	$0
SP_DEC	$2
CALL	vos_dev_open
POP16	%eax
SP_WR16	%eax	$3
SP_INC	$1
PUSH8	$4
SP_DEC	$2
CALL	vos_dev_open
POP16	%eax
SP_WR16	%eax	$5
SP_INC	$1
LD16	%ecx	$0
SP_WR16	%ecx	$6
LD16	%ecx	$0
SP_WR16	%ecx	$8
LD8	%r3	$0
SP_STORE	%ecx
INC16	%ecx	$10
LD32	%eax	$Array@2
CPYROM	(%ecx)	%eax	$6
SP_STORE	%ecx
INC16	%ecx	$22
LD32	%eax	$Array@3
CPYROM	(%ecx)	%eax	$6
SP_STORE	%ecx
INC16	%ecx	$34
LD32	%eax	$Array@4
CPYROM	(%ecx)	%eax	$6
SP_STORE	%ecx
INC16	%ecx	$46
LD32	%eax	$Array@5
CPYROM	(%ecx)	%eax	$6
SP_RD16	%eax	$0
PUSH16	%eax
CALL	uartsetup
SP_INC	$2
@IC15:	
LD8	%ecx	$1
CMP8	%ecx	$0
JZ	@IC16
@IC17:	
SP_STORE	%r1
INC16	%r1	$58
CPY16	%r2	%r1
LD8	(%r2)	$2
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$71
SP_INC	$4
INC16	%r1	$5
CPY16	%r1	(%r1)
SP_WR16	%r1	$6
SP_RD16	%ecx	$6
CMP16	%ecx	$0
JLE	@IC18
@IC19:	
SP_STORE	%r1
INC16	%r1	$68
SP_STORE	%r2
INC16	%r2	$8
PUSH16	%r2
SP_RD16	%eax	$8
PUSH16	%eax
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_read
SP_STORE	%eax
ADD16	%eax	$589
POP8	(%eax)
SP_INC	$8
PUSH16	%r1
SP_DEC	$1
CALL	check_sync
SP_STORE	%eax
ADD16	%eax	$584
POP8	(%eax)
SP_INC	$2
SP_STORE	%ecx
ADD16	%ecx	$581
CMP8	(%ecx)	$0
JZ	@IC23
@IC24:	
SP_STORE	%r1
INC16	%r1	$10
PUSH16	$0
PUSH16	$12
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_write
SP_STORE	%eax
ADD16	%eax	$591
POP8	(%eax)
SP_INC	$8
PUSH16	$1000
SP_DEC	$1
CALL	vos_delay_msecs
SP_STORE	%eax
ADD16	%eax	$586
POP8	(%eax)
SP_INC	$2
SP_STORE	%r1
ADD16	%r1	$22
PUSH16	$0
PUSH16	$12
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_write
SP_STORE	%eax
ADD16	%eax	$593
POP8	(%eax)
SP_INC	$8
JUMP	@IC22
@IC23:	
SP_STORE	%r1
ADD16	%r1	$46
PUSH16	$0
PUSH16	$12
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_write
SP_STORE	%eax
ADD16	%eax	$594
POP8	(%eax)
SP_INC	$8
@IC22:	
LD16	%ecx	$0
SP_WR16	%ecx	$6
@IC18:	
CMP8	%r3	$255
JNZ	@IC25
@IC26:	
SP_STORE	%r1
ADD16	%r1	$34
PUSH16	$0
PUSH16	$12
PUSH16	%r1
PUSH16	%r0
SP_DEC	$1
CALL	vos_dev_write
SP_STORE	%eax
ADD16	%eax	$595
POP8	(%eax)
SP_INC	$8
LD8	%r3	$0
@IC25:	
INC8	%r3	$1
PUSH16	$100
SP_DEC	$1
CALL	vos_delay_msecs
SP_STORE	%eax
ADD16	%eax	$590
POP8	(%eax)
SP_INC	$2
JUMP	@IC15
@IC16:	
SP_INC	$255
SP_INC	$255
SP_INC	$78
POP32	%r3
POP32	%r2
POP32	%r1
POP32	%r0
RTS	
.FUNC_END	"reprogramming"

bus:	
.GLOBAL	 DO_NOT_EXPORT  "bus"

.FUNCTION	"bus"	
PUSH32	%r0
PUSH32	%r1
PUSH32	%r2
PUSH32	%r3
SP_DEC	$255
SP_DEC	$255
SP_DEC	$255
SP_DEC	$255
SP_DEC	$64
LD16	%ecx	$0
SP_WR16	%ecx	$0
LD16	%ecx	$0
SP_WR16	%ecx	$2
PUSH8	$2
SP_DEC	$2
CALL	vos_dev_open
POP16	%eax
SP_WR16	%eax	$5
SP_INC	$1
SP_RD16	%ecx	$4
SP_WR16	%ecx	$6
PUSH8	$3
SP_DEC	$2
CALL	vos_dev_open
POP16	%eax
SP_WR16	%eax	$9
SP_INC	$1
SP_RD16	%ecx	$8
SP_WR16	%ecx	$10
SP_STORE	%r0
ADD16	%r0	$12
CPY16	%r1	%r0
LD8	(%r1)	$65
LD16	%r1	$1
ADD16	%r1	%r0
CPY16	%r2	%r1
LD8	(%r2)	$0
PUSH16	%r0
SP_RD16	%eax	$6
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$25
SP_INC	$4
CPY16	%r2	%r0
LD8	(%r2)	$66
CPY16	%r2	%r1
LD8	(%r2)	$0
PUSH16	%r0
SP_RD16	%eax	$6
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$26
SP_INC	$4
CPY16	%r2	%r0
LD8	(%r2)	$67
CPY16	%r2	%r1
LD8	(%r2)	$0
PUSH16	%r0
SP_RD16	%eax	$6
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$27
SP_INC	$4
CPY16	%r2	%r0
LD8	(%r2)	$68
CPY16	%r2	%r1
LD8	(%r2)	$0
PUSH16	%r0
SP_RD16	%eax	$6
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$28
SP_INC	$4
CPY16	%r2	%r0
LD8	(%r2)	$69
CPY16	%r2	%r1
LD8	(%r2)	$3
PUSH16	%r0
SP_RD16	%eax	$6
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$29
SP_INC	$4
CPY16	%r2	%r0
LD8	(%r2)	$4
LD16	(%r1)	$0
PUSH16	%r0
SP_RD16	%eax	$6
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$30
SP_INC	$4
SP_STORE	%r1
INC16	%r1	$27
CPY16	%r2	%r1
LD8	(%r2)	$65
LD16	%r2	$1
ADD16	%r2	%r1
CPY16	%r3	%r2
LD8	(%r3)	$0
PUSH16	%r1
SP_RD16	%eax	$10
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$40
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$66
CPY16	%r3	%r2
LD8	(%r3)	$0
PUSH16	%r1
SP_RD16	%eax	$10
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$41
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$67
CPY16	%r3	%r2
LD8	(%r3)	$0
PUSH16	%r1
SP_RD16	%eax	$10
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$42
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$68
CPY16	%r3	%r2
LD8	(%r3)	$7
PUSH16	%r1
SP_RD16	%eax	$10
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$43
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$69
CPY16	%r3	%r2
LD8	(%r3)	$3
PUSH16	%r1
SP_RD16	%eax	$10
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$44
SP_INC	$4
CPY16	%r3	%r1
LD8	(%r3)	$4
LD16	(%r2)	$0
PUSH16	%r1
SP_RD16	%eax	$10
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$45
SP_INC	$4
CPY16	%r2	%r0
LD8	(%r2)	$1
PUSH16	%r0
SP_RD16	%eax	$6
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$46
SP_INC	$4
CPY16	%r0	%r1
LD8	(%r0)	$1
PUSH16	%r1
SP_RD16	%eax	$10
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$47
SP_INC	$4
@IC29:	
LD8	%ecx	$1
CMP8	%ecx	$0
JZ	@IC30
@IC31:	
PUSH16	$10
SP_DEC	$1
CALL	vos_delay_msecs
POP8	%eax
SP_WR8	%eax	$46
SP_INC	$2
SP_STORE	%r0
INC16	%r0	$12
CPY16	%r1	%r0
LD8	(%r1)	$2
PUSH16	%r0
SP_RD16	%eax	$8
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$49
SP_INC	$4
PUSH16	$100
SP_DEC	$1
CALL	vos_delay_msecs
POP8	%eax
SP_WR8	%eax	$48
SP_INC	$2
INC16	%r0	$5
CPY16	%r0	(%r0)
SP_WR16	%r0	$0
SP_STORE	%r1
INC16	%r1	$27
CPY16	%r2	%r1
LD8	(%r2)	$3
PUSH16	%r1
SP_RD16	%eax	$12
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
POP8	%eax
SP_WR8	%eax	$51
SP_INC	$4
INC16	%r1	$5
CPY16	%r1	(%r1)
SP_WR16	%r1	$2
CMP16	%r0	$0
JZ	@IC32
@IC33:	
SP_STORE	%r0
INC16	%r0	$48
PUSH16	$0
SP_RD16	%eax	$2
PUSH16	%eax
PUSH16	%r0
SP_RD16	%eax	$12
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_read
SP_STORE	%eax
ADD16	%eax	$569
POP8	(%eax)
SP_INC	$8
PUSH16	$10
SP_DEC	$1
CALL	vos_delay_msecs
SP_STORE	%eax
ADD16	%eax	$564
POP8	(%eax)
SP_INC	$2
PUSH16	%r0
SP_DEC	$1
CALL	check_sync
SP_STORE	%eax
ADD16	%eax	$565
POP8	(%eax)
SP_INC	$2
SP_STORE	%ecx
ADD16	%ecx	$562
CMP8	(%ecx)	$0
JZ	@IC34
@IC36:	
SP_RD16	%ecx	$2
CMP16	%ecx	$0
JNZ	@IC34
@IC35:	
SP_STORE	%r0
INC16	%r0	$48
PUSH16	$0
SP_RD16	%eax	$2
PUSH16	%eax
PUSH16	%r0
SP_RD16	%eax	$16
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_write
SP_STORE	%eax
ADD16	%eax	$572
POP8	(%eax)
SP_INC	$8
@IC34:	
LD16	%ecx	$0
SP_WR16	%ecx	$0
@IC32:	
PUSH16	$10
SP_DEC	$1
CALL	vos_delay_msecs
SP_STORE	%eax
ADD16	%eax	$567
POP8	(%eax)
SP_INC	$2
SP_STORE	%r0
ADD16	%r0	$27
CPY16	%r1	%r0
LD8	(%r1)	$2
PUSH16	%r0
SP_RD16	%eax	$12
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
SP_STORE	%eax
ADD16	%eax	$570
POP8	(%eax)
SP_INC	$4
INC16	%r0	$5
CPY16	%r0	(%r0)
SP_WR16	%r0	$0
PUSH16	$10
SP_DEC	$1
CALL	vos_delay_msecs
SP_STORE	%eax
ADD16	%eax	$569
POP8	(%eax)
SP_INC	$2
SP_STORE	%r1
ADD16	%r1	$12
CPY16	%r2	%r1
LD8	(%r2)	$3
PUSH16	%r1
SP_RD16	%eax	$8
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_ioctl
SP_STORE	%eax
ADD16	%eax	$572
POP8	(%eax)
SP_INC	$4
INC16	%r1	$5
CPY16	%r1	(%r1)
SP_WR16	%r1	$2
CMP16	%r0	$0
JZ	@IC29
@IC40:	
SP_STORE	%r0
ADD16	%r0	$568
PUSH16	$0
SP_RD16	%eax	$2
PUSH16	%eax
PUSH16	%r0
SP_RD16	%eax	$16
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_read
SP_STORE	%eax
ADD16	%eax	$1089
POP8	(%eax)
SP_INC	$8
PUSH16	$10
SP_DEC	$1
CALL	vos_delay_msecs
SP_STORE	%eax
ADD16	%eax	$1084
POP8	(%eax)
SP_INC	$2
PUSH16	%r0
SP_DEC	$1
CALL	check_sync
SP_STORE	%eax
ADD16	%eax	$1085
POP8	(%eax)
SP_INC	$2
SP_STORE	%ecx
ADD16	%ecx	$1082
CMP8	(%ecx)	$0
JZ	@IC41
@IC43:	
SP_RD16	%ecx	$2
CMP16	%ecx	$0
JNZ	@IC41
@IC42:	
SP_STORE	%r0
ADD16	%r0	$568
PUSH16	$0
SP_RD16	%eax	$2
PUSH16	%eax
PUSH16	%r0
SP_RD16	%eax	$12
PUSH16	%eax
SP_DEC	$1
CALL	vos_dev_write
SP_STORE	%eax
ADD16	%eax	$1092
POP8	(%eax)
SP_INC	$8
@IC41:	
LD16	%ecx	$0
SP_WR16	%ecx	$0
@IC39:	
JUMP	@IC29
@IC30:	
SP_INC	$255
SP_INC	$255
SP_INC	$255
SP_INC	$255
SP_INC	$64
POP32	%r3
POP32	%r2
POP32	%r1
POP32	%r0
RTS	
.FUNC_END	"bus"

