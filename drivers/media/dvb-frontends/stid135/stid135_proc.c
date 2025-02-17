/*
 *
* Copyright (c) <2021>-<2022>, Deep Thought, all rights reserved
* Author(s): deeptho@gmail.com
 *
* License terms: BSD 3-clause "New" or "Revised" License.
 *
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h> /* min */
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> /* copy_from_user, copy_to_user */
#include <linux/slab.h>
#include <linux/seq_file.h>
#include "i2c.h"
#include "chip.h"

STCHIP_Info_t* hChipHandle_stid135;
STCHIP_Info_t* hChipHandle_soc;
STCHIP_Info_t* hChipHandle_vglna;


static int open_stid135(struct inode *inode, struct file *filp)
{
    pr_info("open\n");
		//pr_info("virt_to_phys = 0x%llx\n", (unsigned long long)virt_to_phys((void *)info));
    filp->private_data = (char*) hChipHandle_stid135;
    return 0;
}

static int open_soc(struct inode *inode, struct file *filp)
{
    pr_info("open\n");
		//pr_info("virt_to_phys = 0x%llx\n", (unsigned long long)virt_to_phys((void *)info));
    filp->private_data = (char*) hChipHandle_soc;
    return 0;
}

static int open_vglna(struct inode *inode, struct file *filp)
{
    pr_info("open\n");
		//pr_info("virt_to_phys = 0x%llx\n", (unsigned long long)virt_to_phys((void *)info));
    filp->private_data = (char*) hChipHandle_vglna;
    return 0;
}


static ssize_t read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
    int ret;
		STCHIP_Info_t* hChipHandle = filp->private_data;
		size_t hChipSize = hChipHandle->NbRegs*sizeof(STCHIP_Register_t);
		pr_info("hCip proc read\n");
		if(*off>0)
			return 0;
		pr_info("read off=%lld len=%ld\n", *off, len);

    ret = min(len, hChipSize);

		if (copy_to_user(buf, hChipHandle->pRegMapImage, ret)) {
        ret = -EFAULT;
    }
		*off=ret;
    return ret;
}


static int release(struct inode *inode, struct file *filp)
{
    pr_info("release\n");
    //free_page((unsigned long)info->data);
    //kfree(info);
    filp->private_data = NULL;
    return 0;
}

static const struct proc_ops fops_stid135 = {
	.proc_lseek = seq_lseek,
	.proc_open = open_stid135,
	.proc_release = release,
    .proc_read = read,
};


static const struct proc_ops fops_soc = {
	.proc_lseek = seq_lseek,
	.proc_open = open_soc,
	.proc_release = release,
    .proc_read = read,
};


static const struct proc_ops fops_vglna = {
	.proc_lseek = seq_lseek,
	.proc_open = open_vglna,
	.proc_release = release,
    .proc_read = read,
};



void chip_init_proc(STCHIP_Info_t* hChipHandle_, const char* name)
{
	char filename[256];
	sprintf(filename, "hchip_%s",name);
	if(strcmp(name, "stid135")==0) {
		proc_create(filename, 0777, NULL, &fops_stid135);
		hChipHandle_stid135 = hChipHandle_;
	} else if(strcmp(name, "vglna")==0) {
		proc_create(filename, 0777, NULL, &fops_vglna);
		hChipHandle_vglna = hChipHandle_;
	} else {
		proc_create(filename, 0777, NULL, &fops_soc);
		hChipHandle_soc = hChipHandle_;
	}
}

void chip_close_proc(const char*name)
{
	char filename[256];
	sprintf(filename, "hchip_%s",name);
	 remove_proc_entry(filename, NULL);
}
