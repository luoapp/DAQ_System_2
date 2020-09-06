#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
};

static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
	{ 0x2b243a8e, "struct_module" },
	{ 0x3116e31e, "cdev_del" },
	{ 0x31bdb298, "pci_bus_read_config_byte" },
	{ 0x12da5bb2, "__kmalloc" },
	{ 0x5b93cd33, "cdev_init" },
	{ 0x60253ab0, "up_read" },
	{ 0x740acdec, "mem_map" },
	{ 0xe1b7029c, "print_tainted" },
	{ 0x458a03ff, "malloc_sizes" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0x7d11c268, "jiffies" },
	{ 0xd533bec7, "__might_sleep" },
	{ 0xf0c7cc95, "down_read" },
	{ 0xd7474566, "__copy_to_user_ll" },
	{ 0xda4008e6, "cond_resched" },
	{ 0x1b7d4074, "printk" },
	{ 0x5f9d211f, "kunmap" },
	{ 0x520a5172, "fasync_helper" },
	{ 0x8649b199, "dma_free_coherent" },
	{ 0x9eac042a, "__ioremap" },
	{ 0xe1717cda, "dma_alloc_coherent" },
	{ 0x71b99850, "cdev_add" },
	{ 0x426ae467, "kmem_cache_alloc" },
	{ 0xa88f50c9, "pci_bus_read_config_word" },
	{ 0x6f0a1ad0, "kmap" },
	{ 0xaf3e78c2, "get_user_pages" },
	{ 0x2cf190e3, "request_irq" },
	{ 0x4292364c, "schedule" },
	{ 0x6b2dc060, "dump_stack" },
	{ 0x85b94803, "pci_unregister_driver" },
	{ 0xffd3c7, "init_waitqueue_head" },
	{ 0x37a0cba, "kfree" },
	{ 0xedc03953, "iounmap" },
	{ 0x8db1bccd, "__pci_register_driver" },
	{ 0x1ffa2ca3, "put_page" },
	{ 0x50880a70, "kill_fasync" },
	{ 0x60a4461c, "__up_wakeup" },
	{ 0x96b27088, "__down_failed" },
	{ 0x908aa9b2, "iowrite32" },
	{ 0x73f0922d, "pci_enable_device" },
	{ 0xf2a644fb, "copy_from_user" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x2241a928, "ioread32" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v000010B5d00009056sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "AA776042E879085F83C9135");
