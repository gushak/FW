#include "iodefine.h"

/* field table */

enum if_1_list {
    _std_con_prd_no = 0,        // 0
    _std_prc_no,                // 1
    _std_prc_class_cd,          // 2
    _std_prc_nm,                // 3
    _std_prc_mea_cd,            // 4
    _std_tot_prc_tm,            // 5
    _std_tot_mc_tm,             // 6
    _std_piece_no,              // 7
    _std_stitch_no,             // 8
    _std_seam_no,               // 9
    _std_margin_width,          // 10
    _std_prc_level,             // 11
    _std_person_cnt,            // 12
    _std_remark,                // 13
    _std_voice_file_url,        // 14
    _std_movie_file_url,        // 15
    _std_sub_prc_num,           // 16
    _std_con_prd_nm,            // 17
#ifdef NEW_WORK_CMD
    _std_skill_grade,   //(130118)    // 18
    _std_trim_cnt,      //(130118)    // 19
#endif
};

enum if_2_list {
	_sew_con_prd_no = 0,
	_sew_prc_no,
	_sew_sub_prc_no,
	_sew_ptn_num,
	_sew_sub_ptn_num,
	_sew_prc_ssd_file_url,
	_sew_machine_tp_cd,
	_sew_speed,
	_sew_stitch,
	_sew_nrb_press,
	_sew_trd_tns1,
	_sew_ndl_gap,
	_sew_memory_ptn,
	_sew_ratio_x,
	_sew_ratio_y,
	_sew_exp_ptn_num,
	_sew_chn_num,
	_sew_up_trd_tp1,
	_sew_up_trd_spec1,
	_sew_up_trd_clr1,
	_sew_dn_trd_tp1,
	_sew_dn_trd_spec1,
	_sew_dn_trd_clr1,
	_sew_ndl_tp1,
	_sew_ndl_spec1,
};

enum if_3_list {
	_cmd_con_prd_no,
	_cmd_con_prd_nm,
	_cmd_prc_no,
	_cmd_prc_nm,
	_cmd_tot_prd_amt,
	_cmd_tot_prd_day,
	_cmd_acc_prd_amt,
	_cmd_acc_prd_day,
	_cmd_work_dt,
	_cmd_mbr_id,
	_cmd_emp_nm,
	_cmd_b_prc_no,
	_cmd_b_prc_amt,
	_cmd_f_prc_no,
	_cmd_f_prc_amt,
	_cmd_today_alloc_amt,
	_cmd_today_add_amt,
	_cmd_today_tot_alloc_amt,
	_cmd_remark,
	_cmd_voice_path_url,
	_cmd_movie_path_url,
	_cmd_mc_senro,
};

