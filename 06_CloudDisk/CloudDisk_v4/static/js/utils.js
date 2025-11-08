/**
 * utils.js - 工具函数库
 * 包含常用的格式化、验证等工具函数
 */

// 文件大小格式化
function formatFileSize(bytes) {
  if (bytes === 0) return '0 B';
  const k = 1024;
  const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
  const i = Math.floor(Math.log(bytes) / Math.log(k));
  return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

// 日期格式化
function formatDate(dateString) {
  if (!dateString) return '-';
  const date = new Date(dateString);
  const year = date.getFullYear();
  const month = String(date.getMonth() + 1).padStart(2, '0');
  const day = String(date.getDate()).padStart(2, '0');
  const hours = String(date.getHours()).padStart(2, '0');
  const minutes = String(date.getMinutes()).padStart(2, '0');
  return `${year}-${month}-${day} ${hours}:${minutes}`;
}

// 根据文件名获取文件图标
function getFileIcon(filename) {
  if (!filename) return 'fa-file';

  const ext = filename.split('.').pop().toLowerCase();
  const iconMap = {
    // 图片
    'jpg': 'fa-file-image',
    'jpeg': 'fa-file-image',
    'png': 'fa-file-image',
    'gif': 'fa-file-image',
    'bmp': 'fa-file-image',
    'svg': 'fa-file-image',
    'webp': 'fa-file-image',

    // 文档
    'pdf': 'fa-file-pdf',
    'doc': 'fa-file-word',
    'docx': 'fa-file-word',
    'xls': 'fa-file-excel',
    'xlsx': 'fa-file-excel',
    'ppt': 'fa-file-powerpoint',
    'pptx': 'fa-file-powerpoint',
    'txt': 'fa-file-lines',
    'md': 'fa-file-lines',

    // 代码
    'html': 'fa-file-code',
    'css': 'fa-file-code',
    'js': 'fa-file-code',
    'json': 'fa-file-code',
    'xml': 'fa-file-code',
    'cpp': 'fa-file-code',
    'c': 'fa-file-code',
    'h': 'fa-file-code',
    'py': 'fa-file-code',
    'java': 'fa-file-code',
    'php': 'fa-file-code',

    // 压缩包
    'zip': 'fa-file-zipper',
    'rar': 'fa-file-zipper',
    '7z': 'fa-file-zipper',
    'tar': 'fa-file-zipper',
    'gz': 'fa-file-zipper',

    // 音频
    'mp3': 'fa-file-audio',
    'wav': 'fa-file-audio',
    'flac': 'fa-file-audio',
    'aac': 'fa-file-audio',

    // 视频
    'mp4': 'fa-file-video',
    'avi': 'fa-file-video',
    'mkv': 'fa-file-video',
    'mov': 'fa-file-video',
    'wmv': 'fa-file-video'
  };

  return iconMap[ext] || 'fa-file';
}

// 密码强度检查
function checkPasswordStrength(password) {
  if (!password) return { strength: 0, text: '请输入密码', color: 'var(--text-muted)' };

  let strength = 0;

  if (password.length >= 8) strength++;
  if (password.length >= 12) strength++;
  if (/[a-z]/.test(password)) strength++;
  if (/[A-Z]/.test(password)) strength++;
  if (/[0-9]/.test(password)) strength++;
  if (/[^a-zA-Z0-9]/.test(password)) strength++;

  if (strength <= 2) {
    return { strength: 1, text: '弱', color: 'var(--danger-color)' };
  } else if (strength <= 4) {
    return { strength: 2, text: '中', color: 'var(--warning-color)' };
  } else {
    return { strength: 3, text: '强', color: 'var(--success-color)' };
  }
}

// 表单验证
function validateForm(formData) {
  const errors = {};

  if (!formData.username || formData.username.trim() === '') {
    errors.username = '用户名不能为空';
  }

  if (!formData.password || formData.password.trim() === '') {
    errors.password = '密码不能为空';
  } else if (formData.password.length < 6) {
    errors.password = '密码至少6个字符';
  }

  return {
    isValid: Object.keys(errors).length === 0,
    errors: errors
  };
}

// 防抖函数
function debounce(func, wait) {
  let timeout;
  return function executedFunction(...args) {
    const later = () => {
      clearTimeout(timeout);
      func(...args);
    };
    clearTimeout(timeout);
    timeout = setTimeout(later, wait);
  };
}

// 节流函数
function throttle(func, limit) {
  let inThrottle;
  return function() {
    const args = arguments;
    const context = this;
    if (!inThrottle) {
      func.apply(context, args);
      inThrottle = true;
      setTimeout(() => inThrottle = false, limit);
    }
  };
}

// 复制到剪贴板
function copyToClipboard(text) {
  if (navigator.clipboard && navigator.clipboard.writeText) {
    return navigator.clipboard.writeText(text);
  } else {
    // 降级方案
    const textarea = document.createElement('textarea');
    textarea.value = text;
    textarea.style.position = 'fixed';
    textarea.style.opacity = '0';
    document.body.appendChild(textarea);
    textarea.select();
    document.execCommand('copy');
    document.body.removeChild(textarea);
    return Promise.resolve();
  }
}
