
#ifndef __l2cache_defs_h
#define __l2cache_defs_h

/* Main access macros */
#ifndef REG_RD
#define REG_RD( scope, inst, reg ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR
#define REG_WR( scope, inst, reg, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_VECT
#define REG_RD_VECT( scope, inst, reg, index ) \
  REG_READ( reg_##scope##_##reg, \
            (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_VECT
#define REG_WR_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( reg_##scope##_##reg, \
             (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT
#define REG_RD_INT( scope, inst, reg ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_WR_INT
#define REG_WR_INT( scope, inst, reg, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg, (val) )
#endif

#ifndef REG_RD_INT_VECT
#define REG_RD_INT_VECT( scope, inst, reg, index ) \
  REG_READ( int, (inst) + REG_RD_ADDR_##scope##_##reg + \
	    (index) * STRIDE_##scope##_##reg )
#endif

#ifndef REG_WR_INT_VECT
#define REG_WR_INT_VECT( scope, inst, reg, index, val ) \
  REG_WRITE( int, (inst) + REG_WR_ADDR_##scope##_##reg + \
	     (index) * STRIDE_##scope##_##reg, (val) )
#endif

#ifndef REG_TYPE_CONV
#define REG_TYPE_CONV( type, orgtype, val ) \
  ( { union { orgtype o; type n; } r; r.o = val; r.n; } )
#endif

#ifndef reg_page_size
#define reg_page_size 8192
#endif

#ifndef REG_ADDR
#define REG_ADDR( scope, inst, reg ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg )
#endif

#ifndef REG_ADDR_VECT
#define REG_ADDR_VECT( scope, inst, reg, index ) \
  ( (inst) + REG_RD_ADDR_##scope##_##reg + \
    (index) * STRIDE_##scope##_##reg )
#endif

/* C-code for register scope l2cache */

/* Register rw_cfg, scope l2cache, type rw */
typedef struct {
  unsigned int en : 1;
  unsigned int dummy1 : 31;
} reg_l2cache_rw_cfg;
#define REG_RD_ADDR_l2cache_rw_cfg 0
#define REG_WR_ADDR_l2cache_rw_cfg 0

/* Register rw_ctrl, scope l2cache, type rw */
typedef struct {
  unsigned int dummy1 : 7;
  unsigned int cbase : 9;
  unsigned int dummy2 : 4;
  unsigned int csize : 10;
  unsigned int dummy3 : 2;
} reg_l2cache_rw_ctrl;
#define REG_RD_ADDR_l2cache_rw_ctrl 4
#define REG_WR_ADDR_l2cache_rw_ctrl 4

/* Register rw_idxop, scope l2cache, type rw */
typedef struct {
  unsigned int idx : 10;
  unsigned int dummy1 : 14;
  unsigned int way : 3;
  unsigned int dummy2 : 2;
  unsigned int cmd : 3;
} reg_l2cache_rw_idxop;
#define REG_RD_ADDR_l2cache_rw_idxop 8
#define REG_WR_ADDR_l2cache_rw_idxop 8

/* Register rw_addrop_addr, scope l2cache, type rw */
typedef struct {
  unsigned int addr : 32;
} reg_l2cache_rw_addrop_addr;
#define REG_RD_ADDR_l2cache_rw_addrop_addr 12
#define REG_WR_ADDR_l2cache_rw_addrop_addr 12

/* Register rw_addrop_ctrl, scope l2cache, type rw */
typedef struct {
  unsigned int size : 16;
  unsigned int dummy1 : 13;
  unsigned int cmd  : 3;
} reg_l2cache_rw_addrop_ctrl;
#define REG_RD_ADDR_l2cache_rw_addrop_ctrl 16
#define REG_WR_ADDR_l2cache_rw_addrop_ctrl 16


/* Constants */
enum {
  regk_l2cache_flush                       = 0x00000001,
  regk_l2cache_no                          = 0x00000000,
  regk_l2cache_rw_addrop_addr_default      = 0x00000000,
  regk_l2cache_rw_addrop_ctrl_default      = 0x00000000,
  regk_l2cache_rw_cfg_default              = 0x00000000,
  regk_l2cache_rw_ctrl_default             = 0x00000000,
  regk_l2cache_rw_idxop_default            = 0x00000000,
  regk_l2cache_yes                         = 0x00000001
};
#endif /* __l2cache_defs_h */
